#!/bin/bash

if [ -n "$MD5SRV_PORT" ]; then
    echo "INFO: Using MD5 server port $MD5SRV_PORT." >&2
else
    echo "ERROR: MD5SRV_PORT not defined." >&2
    exit 1
fi

if ! which ngrok > /dev/null 2>&1; then
    echo "ERROR: ngrok command not found." >&2
    exit 1
fi

if ! which jq > /dev/null 2>&1; then
    echo "ERROR: jq command not found." >&2
    exit 1
fi

echo "INFO: Starting ngrok..." >&2
ngrok http http://localhost:$MD5SRV_PORT --log-format json --log stdout > ngrok.log 2>&1 &
NGROK_PID=$!
echo "INFO: ngrok PID is $NGROK_PID." >&2
# echo "TMOLE_PID=$TMOLE_PID" >> $GITHUB_ENV
timeout 10 bash -c "until test -e ngrok.log; do sleep 0.1; done"
if [ $? -ne 0 ]; then
    echo "ERROR: ngrok failed to start." >&2
    exit 1
fi

sleep 1

TIMEOUT=8
SECONDS=0
MD5SRV_URL=""
# Check ngrok.log for the MD5 server URL every second.
until [ "$SECONDS" -ge "$TIMEOUT" ]
do
    MD5SRV_URL=$(jq -r 'select(.msg == "started tunnel") | .url' ngrok.log)
    if [ -n "$MD5SRV_URL" ]; then
        break
    else
        SECONDS=$((SECONDS+1))
        sleep 1
    fi
done

if [ -z "$MD5SRV_URL" ]; then
    echo "ERROR: Timed out waiting for MD5 server URL to get written to ngrok.log." >&2
    exit 1
fi

echo "INFO: Got MD5 server URL from ngrok.log." >&2
echo "MD5SRV_URL=$MD5SRV_URL" >> $GITHUB_ENV
echo "INFO: ngrok ready. Logging to `realpath ngrok.log`" >&2
