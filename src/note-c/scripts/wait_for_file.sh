#!/bin/bash
# $1 filename to wait for
set -e

if [ -z "$1" ]; then
  echo "Expected 1 argument: <filename>"
  exit 1
else
  echo "Waiting for file $1..."
fi

while ! test -e "$1"; do
  sleep 0.5
done
