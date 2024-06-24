#!/bin/bash

#
# Update the note-c subtree.
#
# Usage: ./update_note_c.sh <note-c git ref>
#
# The note-c git ref argument is optional. If not specified, the master branch
# will be used.
#

# Exit script if any command fails.
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$SCRIPT_DIR/.."

# Optional argument to specify the note-c ref to check out when creating the
# subtree.
if [ $# -eq 0 ]; then
    REF="master"
else
    REF="$1"
fi

# Move to the root of note-arduino.
pushd $ROOT_DIR
# Remove the existing note-c directory.
rm -rf src/note-c
# Commit the changes so that the next git subtree command doesn't complain about
# changes in the working tree.
git commit -am 'Remove `note-c` before re-add'
# Check out the note-c subtree using the given ref or master if not specified.
git subtree add --prefix=src/note-c --squash https://github.com/blues/note-c.git $REF

# Remove all the unneeded directories from the subtree.
NOTE_C_DIR="$ROOT_DIR/src/note-c"
NOTE_C_UNNEEDED_DIRS=(
    "$NOTE_C_DIR/.devcontainer"
    "$NOTE_C_DIR/.github"
    "$NOTE_C_DIR/.vscode"
    "$NOTE_C_DIR/assets"
    "$NOTE_C_DIR/docs"
    "$NOTE_C_DIR/scripts"
    "$NOTE_C_DIR/test"
)
for DIR in "${NOTE_C_UNNEEDED_DIRS[@]}"; do
    rm -rf "$DIR"
done

# Commit the removal of unneeded directories.
git commit -am 'Remove unneeded directories from `note-c`'

# Return to original directory.
popd
