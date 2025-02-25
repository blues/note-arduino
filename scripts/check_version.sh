#!/bin/bash
set -e

# Script to check that the version in src/NoteDefines.h matches the version in library.properties
# Usage: ./check_version.sh

# Get the version from library.properties
LIBRARY_VERSION=$(grep "^version=" library.properties | cut -d= -f2)

# Get the version components from NoteDefines.h
MAJOR=$(grep "NOTE_ARDUINO_VERSION_MAJOR" src/NoteDefines.h | grep -o "[0-9]\+")
MINOR=$(grep "NOTE_ARDUINO_VERSION_MINOR" src/NoteDefines.h | grep -o "[0-9]\+")
PATCH=$(grep "NOTE_ARDUINO_VERSION_PATCH" src/NoteDefines.h | grep -o "[0-9]\+")

# Construct the version string from the components
DEFINES_VERSION="$MAJOR.$MINOR.$PATCH"

echo "Version in library.properties: $LIBRARY_VERSION"
echo "Version in src/NoteDefines.h: $DEFINES_VERSION"

# Compare the versions
if [ "$LIBRARY_VERSION" = "$DEFINES_VERSION" ]; then
    echo "✅ Versions match!"
    exit 0
else
    echo "❌ Version mismatch!"
    echo "library.properties version: $LIBRARY_VERSION"
    echo "src/NoteDefines.h version: $DEFINES_VERSION"
    exit 1
fi
