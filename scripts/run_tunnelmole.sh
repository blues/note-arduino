#!/bin/bash

if [ -n "$MD5SRV_PORT" ]; then
    echo "INFO: Using MD5 server port $MD5SRV_PORT." >&2
else
    echo "ERROR: MD5SRV_PORT not defined." >&2
    exit 1
fi

if ! which tmole > /dev/null 2>&1; then
    echo "ERROR: tmole command not found." >&2
    exit 1
fi

echo "INFO: Starting tmole..." >&2
tmole $MD5SRV_PORT > tmole.log 2>&1 &
TMOLE_PID=$!
echo "INFO: tmole PID is $TMOLE_PID." >&2
echo "TMOLE_PID=$TMOLE_PID" >> $GITHUB_ENV
timeout 10 bash -c "until test -e tmole.log; do sleep 0.1; done"
if [ $? -ne 0 ]; then
    echo "ERROR: tmole failed to start." >&2
    exit 1
fi

sleep 1


TIMEOUT=8
SECONDS=0
MD5SRV_URL=""
# Check tmole.log for the MD5 server URL every second.
until [ "$SECONDS" -ge "$TIMEOUT" ]
do
    MD5SRV_URL=$(grep -oP "^https://[\w\d\.-]+" tmole.log)

    if [ -n "$MD5SRV_URL" ]; then
        break
    else
        SECONDS=$((SECONDS+1))
        sleep 1
    fi
done

if [ -z "$MD5SRV_URL" ]; then
    echo "ERROR: Timed out waiting for MD5 server URL to get written to tmole.log." >&2
    exit 1
fi

echo "INFO: Got MD5 server URL from tmole.log." >&2
echo "MD5SRV_URL=$MD5SRV_URL" >> $GITHUB_ENV
echo "INFO: tmole ready. Logging to `realpath tmole.log`" >&2
