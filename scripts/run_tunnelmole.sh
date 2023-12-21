#!/bin/bash
set -e

[ -n "$MD5SRV_PORT" ] || (echo "MD5SRV_PORT not set" && exit 1)
which tmole
tmole $MD5SRV_PORT > tmole.log 2>&1 &
TMOLE_PID=$!
echo "TMOLE_PID=$TMOLE_PID"
echo "TMOLE_PID=$TMOLE_PID" >> $GITHUB_ENV
timeout 10 bash -c "until test -e tmole.log; do sleep 0.1; done"
sleep 1
# find the url
exec 3< tmole.log
# ignore the first line
read <&3 output
read <&3 output
MD5SRV_URL=`echo "$output" | grep https | cut -d " " -f1`
echo "MD5SRV_URL=$MD5SRV_URL"
[ -n "$MD5SRV_URL" ] || (echo "Could not fetch tunnelmole URL" && exit 1)
echo "MD5SRV_URL=$MD5SRV_URL" >> $GITHUB_ENV
# Only create tmole_ready once MD5SRV_URL has been set, as the next step in the
# GitHub workflow depends on it.
touch tmole_ready
sleep 2
echo "Tunnel mole started. Writing logs to `realpath tmole.log`"
