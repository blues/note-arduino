
## `card.binary`` Tests

The tests in this project aim to exhaustively test the `card.binary` feature on Notecard.

The tests have a number of variations:
* The Notecard communications interface used to send and validate the binary data
  * I2C
  * Serial (LPUART)
  * AUX serial
    * baudrates from 9600 up to 115200
* The size of the chunks of data sent over the interface when doing `card.binary.put`.
* The total size of the image sent, and the contents of the image. Variations include:
  * A repeated value
  * A repeated buffer (an image can repeat another image)
  * A random image from a given seed
* The maximum filled size of the `card.binary` buffer. The maximum size is typically used, although smoke tests and other fast-running tests use a smaller value.
* The size of the chunks used to verify the buffer via `card.binary.get`.
* The onward handler of each filled `card.binary` buffer:
  * web.post chunked: sends the data for each filled buffer to the endpoint. The server computes the combined md5 and length for all chunks received which is validated against the original image md5 and length.
  * web.post unchunked: uses Notehub to combine the separate web.post fragments into a single payload sent to the endpoint. Again, the md5 and length are validated.
  * note.add

## Outline of the binary test

A particular test may use a small image, and only partially fill the card.binary buffer, or it may fill it several times when the image is larger than the maximum fill size of the buffer set in the test. After the buffer is filled, the binary content is sent to Notehub, either via `web.post` or `note.add`.

In outline, the tests operate like this:

```
    while there's more data in the image to send:
        while the card.binary buffer is not full:
            send a chunk of data over the selected Notecard interface
        validate the card.binary buffer:
            while there's more data to retrieve:
                retrieve a chunk the buffer in chunks of a given size
                compare the contents and md5 of the chunk with the corresponding portion of the image
        dispatch the card.binary buffer:
            when web.post chunked:
                send the card.binary buffer via the web.post endpoint immediately
                verify the length and MD5 received
            when web.post unchunked:
                send the card.binary buffer via the web.post as a fragment to be assembled by Notehub. Validate HTTP status is 100 for all but the last fragment.
    the transfer is complete:
        web.post chunked: validate the total length and MD5 received by the md5 server and the number of chunks sent
        web.post unchunked: validate the final web.post returns 200, the total length and MD5 received by the md5 server and the number of chunks sent
```

The tests perform validation at each step, from each chunk sent all the way to verifying successful
transmission of the complete image.


## Local Test Setup

It's of course possible to run the tests locally, such as for debugging or trying out new types of test. See [LOCAL_TESTING.md](./LOCAL_TESTING.md) for more information on this.