#include "helper.h"
#include <Notecard.h>

struct context
{
	size_t buf_size;
	size_t notecard_binary_offset;
};

static struct context prv_ctx;

void setup_binary_buffer(size_t buf_size)
{
	prv_ctx.buf_size = buf_size;
	prv_ctx.notecard_binary_offset = 0;
}

int send_binary_data_to_notecard(const void *data, size_t len)
{
	const char *err = NoteBinaryTransmit((uint8_t *)data, len, prv_ctx.buf_size, prv_ctx.notecard_binary_offset);

	if (err)
	{
		NoteDebugf("Error sending data: %s\n", err);
		// on an error (e.g. buffer is full)
		// flush what is currently in notecard storage to Notehub with a web.post request
		send_binary_data_to_notehub();
		return 1;
	}

	prv_ctx.notecard_binary_offset += len;

	return 0;
}

void send_binary_data_to_notehub(void)
{
	// notecard must be in "continous" mode and connected to notehub first
	const size_t RETRY_COUNT = 10;
	size_t attempt = 0;
	bool connected = false;
	for (connected = false; !connected && attempt < RETRY_COUNT; ++attempt)
	{
		if (J *rsp = NoteRequestResponse(NoteNewRequest("hub.status")))
		{
			// Ensure the transaction doesn't return an error
			if (NoteResponseError(rsp))
			{
				const char *err = JGetString(rsp, "err");
				NoteDebug(err);
				NoteDebug("\n");
				JDelete(rsp);
			}

			connected = JGetBool(rsp, "connected");
			JDelete(rsp);
		}
		if (!connected)
		{
			delay(5000); // Wait 5 seconds before retrying
		}
	}

	// configure the web.post request according to your specifications
	// https://dev.blues.io/notecard/notecard-walkthrough/web-transactions/
	if (connected)
	{
		if (J *req = NoteNewRequest("web.post"))
		{
			JAddStringToObject(req, "route", "ImageTestRoute"); // update with your route name
			// JAddStringToObject(req, "name", "/notehub-images"); // update with remote path (if needed)
			JAddStringToObject(req, "content", "images/jpeg"); // update with your content type
			JAddBoolToObject(req, "binary", true);
			JAddBoolToObject(req, "verify", true);

			if (!NoteRequest(req))
			{
				NoteDebug("Error sending data\n");
			}
		}

		prv_ctx.notecard_binary_offset = 0;
		NoteBinaryReset();
	}
}
