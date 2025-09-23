import traceback
import shutil
import hashlib
import os
import json
import base64
import argparse
from functools import cached_property
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import parse_qsl, urlparse


def log(s: str):
    print(s, flush=True)


log_sensitive_enabled = False


def log_sensitive(s: str):
    if log_sensitive_enabled:
        log(s)


class HTTPException(Exception):

    def __init__(self, status_code, message, detail=None):
        self.status_code = status_code
        self.message = message
        self.detail = detail


class WebRequestHandler(BaseHTTPRequestHandler):
    """
    GET: retrieves the total length and md5 of all chunks in a directory denoted by the URL path. The URL path
      is interpreted relative to the serving directory.
    If the query parameter "note" is present, the request body is parsed as JSON and the payload saved to 
    a file using the `chunk` event body property.
    POST/PUT: saves the body to a file, with an optional `chunk` query parameter for writing one or more chunks. 
    If no chunk number is given, the file `payload.bin` is written to the directory. Otherwise payloadXXXXX.bin is written
    where XXXXX is the zero-padded chunk number.
    Returns a 409 (Conflict) error if the file already exists.

    DELETE: deletes a directory containing one or more chunks.

    Paths are fully sanitized.
    """

    def __init__(self, args, *others) -> None:
        self.args = args
        if self.args is None:
            raise Exception("invalid args")
        super().__init__(*others)

    # caching properties is done on a per request basis so this is safe.
    @cached_property
    def url(self):
        return urlparse(self.path)

    @cached_property
    def query_data(self):
        return dict(parse_qsl(self.url.query))

    @cached_property
    def post_data(self):
        content_length = int(self.headers.get("Content-Length", 0))
        return self.rfile.read(content_length)

    @cached_property
    def form_data(self):
        return dict(parse_qsl(self.post_data.decode("utf-8")))

    def do_GET(self):
        self.do(self.get_md5)

    def do_DELETE(self):
        self.do(self.delete_files)

    def do_POST(self):
        self.do(self.write_file_or_note)

    def do_PUT(self):
        self.do(self.write_file_or_note)

    def send_status(self,
                    code: int,
                    message: str,
                    detail: str | dict | bytes,
                    headers: dict = {}):
        """ Send a status code with a message and optional detail, which is returned as JSON in the response body. """
        self.send_response(code, message)
        for header in headers.items():
            self.send_header(*header)
        self.send_header(
            "Content-Type", "application/json"
            if type(detail) is not bytes else "application/octet-stream")
        self.end_headers()
        detail = detail or message
        if detail is not None:
            if type(detail) is bytes:
                self.wfile.write(detail)
            else:
                key = "err" if code > 400 else "text" if not type(
                    detail) is dict else None
                json_str = json.dumps(detail)
                reply_body = json_str if key is None else f'{{"{key}":{json_str},"code":{code}}}'
                reply_body = reply_body + '\n'
                self.wfile.write(reply_body.encode('utf-8'))

    def do(self, handler):
        """ Handle a request. If an exception is thrown, raises an internal server error. """
        try:
            # print(self.dump_request())
            self.validate_token()
            handler()
        except HTTPException as e:
            self.send_status(e.status_code, e.message, e.detail)
        except Exception as e:
            traceback.print_exc()
            self.send_status(500, "Internal server error.", str(e))

    def validate_token(self):
        requested = self.headers['X-Access-Token'] or ''
        token = self.args.token or ''
        if token and requested != token:
            raise HTTPException(403,
                                "Forbidden") if requested else HTTPException(
                                    401, "Unauthorized")

    def get_md5(self):
        dirname = self.validate_url_path(self.url.path)
        if not os.path.exists(dirname):
            raise HTTPException(404, "Not Found",
                                f"Directory {self.url.path} not found.")

        chunk = self.query_data.get('chunk')
        headers = {}
        if chunk is None:
            response = self.md5_for_directory(dirname)
        else:
            response = self.chunk_content(dirname, chunk)
            headers = {"Content-Length": len(response)}
        self.send_status(200, "ok", response, headers)

    def chunk_content(self, dirname, chunk):
        return self._read_file(dirname, chunk)

    def md5_for_directory(self, dirname):
        files = os.listdir(dirname)
        files.sort()

        md5context = hashlib.md5()
        length = 0
        for filename in files:
            abs_filename = os.path.join(dirname, filename)
            self.validate_path(abs_filename)
            if not os.path.isfile(abs_filename):
                raise HTTPException(
                    403, "Not a file.",
                    f"{os.path.join(self.url.path,filename)} is not a file")

            with open(abs_filename, 'rb') as file:
                data = file.read()
                length = length + len(data)
                md5context.update(data)

        md5str = md5context.hexdigest()
        response = {"md5": md5str, "length": length, "chunks": len(files)}
        return response

    def delete_files(self):
        dirname = self.validate_url_path(self.url.path)
        if os.path.exists(dirname):
            log(f"deleting {self.url.path}")
            shutil.rmtree(dirname)
        self.send_status(200, "Ok", f"Deleted {self.url.path}")

    def write_file_or_note(self):
        if self.query_data.get('note') is not None:
            self.do(self.write_note)
        else:
            self.do(self.write_file)

    def write_note(self):
        content_type = self.headers['Content-Type']
        if content_type != 'application/json':
            raise HTTPException(400,
                                f"Unsupported content type: {content_type}")
        data = self.post_data
        try:
            event = json.loads(data)
        except json.decoder.JSONDecodeError as e:
            print(f"JSON decode error: {data} {e}")
            raise e
        body = event['body']  # non-optional keys
        name = body['name']
        length = body['length']
        md5 = body['md5']
        # todo - could check the offset
        payload = base64.b64decode(event['payload'])
        payload_length = event.get('payload_length')
        if payload_length is not None and payload_length != length:
            raise HTTPException(
                400, "Payload length mismatch",
                f"payload_length {payload_length}!=length {length}")
        chunk = body.get('chunk')  # optional
        self._write_file(name, chunk, length, payload, md5)

    def write_file(self):
        length = int(self.headers['Content-Length'])
        if not length:
            raise HTTPException(400, "Request body is empty.")
        chunk = self.query_data.get("chunk", None)
        return self._write_file(self.url.path, chunk, length, self.post_data,
                                None)

    def _read_file(self, path, chunk):
        filename = self._chunk_file(path, chunk)
        if not os.path.exists(filename):
            raise HTTPException(404, 'Not found')
        with open(filename, 'rb') as output_file:
            data = output_file.read()
        return data

    def _chunk_file(self, dirname, chunk):
        chunk_index = None if chunk is None else int(chunk)
        filename = os.path.join(
            dirname, f"payload{chunk_index:05d}.bin"
            if chunk_index is not None else "payload.bin")
        self.validate_path(filename)
        return filename

    def _write_file(self, path, chunk, length, data, md5):
        """ Write the request body to a file and returns the MD5 """
        dirname = self.validate_url_path(path)
        if self.args.save:
            if not os.path.exists(dirname):
                log_sensitive(f"Creating directory {dirname}")
                log(f"creating directory {path}")
                os.makedirs(dirname)

            filename = self._chunk_file(dirname, chunk)
            if os.path.exists(filename):
                raise HTTPException(
                    409, "Conflict",
                    f"Chunk {chunk} already exists for {self.url.path}"
                    if chunk else f"File {self.url.path} already exists.")

        if len(data) != length:
            raise HTTPException(
                400, "Invalid content length.",
                f"Payload length does not equal given length {len(data)}!={length}"
            )

        if self.args.save:
            with open(filename, 'wb') as output_file:
                output_file.write(data)
        md5str = hashlib.md5(data).hexdigest()
        if md5 and md5str != md5:
            raise HTTPException(
                400, f"MD5 mismatch. actual {md5str}!=expected {md5}")
        response = {"md5": md5str, "length": length}
        content_type = self.headers['Content-Type']
        if content_type is not None:
            response['Content-Type'] = content_type
        self.send_status(200, "Ok", response)

    def validate_url_path(self, urlpath):
        return self.validate_path(
            os.path.join(self.args.directory, urlpath.lstrip('/')))

    def validate_path(self, path):
        abs_path = os.path.realpath(os.path.abspath(path))
        common = os.path.commonpath((abs_path, self.args.directory))
        if common != self.args.directory:
            raise HTTPException(401, "Denied", "Take a hike.")
        return abs_path

    def dump_request(self) -> str:
        return json.dumps({
            "path": self.url.path,
            "query_data": self.query_data,
            "post_data": self.post_data.decode("utf-8"),
            "form_data": self.form_data
        })


def main(args):
    if args.save and not args.token:
        raise Exception("Token must be provided when files are saved.")

    if args.save and not os.path.exists(args.directory):
        raise OSError("Directory does not exist.")

    log(f'Serving directory {args.directory} at {args.address}:{args.port}')

    def build_request_handler(*request_args):
        return WebRequestHandler(args, *request_args)

    server = HTTPServer((args.address, int(args.port)), build_request_handler)
    server.serve_forever()


if __name__ == "__main__":
    port = os.environ.get("MD5SRV_PORT")
    address = os.environ.get("MD5SRV_ADDRESS")
    token = os.environ.get("MD5SRV_TOKEN")

    parser = argparse.ArgumentParser(
        description=
        'Run a simple webserver to save and validate web.post requests.')
    parser.add_argument('--port',
                        default=port or "8080",
                        required=False,
                        help='The TCP port to bind to.')
    parser.add_argument('--address',
                        default=address or "0.0.0.0",
                        required=False,
                        help='The IP address to bind to.')
    parser.add_argument(
        '--dir',
        dest="directory",
        required=False,
        help=
        'The working directory. All files are stored and retrieved from here.')
    # parser.add_argument('--timeout',
    #                     default=5,
    #                     required=False,
    #                     help='Set the request timeout.')
    parser.add_argument('--save',
                        default=False,
                        required=False,
                        action='store_true',
                        help='Save the content received to the filesystem.')
    parser.add_argument(
        '--token',
        default=None,
        required=False,
        help='The authorization token required in X-Access-Token header.')

    args = parser.parse_args()
    args.token = args.token or token
    args.directory = os.getcwd() if not args.directory else os.path.abspath(
        args.directory)
    main(args)
