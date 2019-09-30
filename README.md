# [Blues Wireless][blues]

The note-arduino Arduino library for communicating with Blues Wireless Notecard via serial or I²C. Includes example sketches in the [examples directory](examples).

This library allows you to control a Notecard by writing an Arduino sketch in C or C++.
Your sketch may programmatically configure Notecard and send Notes to [Notehub.io][notehub].

This library is a wrapper around the [note-c library][note-c], which it includes as a git submodule.

This library is used by the [m5stack repo][m5stack] m5note Arduino sketch, which requires it to be installed per the instructions below.

## Installation

1. Download the library as a zip from [here][archive] or use the
UI `Clone or Download` -> `Download ZIP`
2. Start the Arduino IDE
3. Import the library:
   ```
   Sketch -> Include library -> Add ZIP Library
   ```
And select the previously downloaded zip file.

## Keeping up to date with note-c repo

This library depends on the blues [note-c repo][note-c] and utilizes
git subtrees to include those files in the src/note-c folder. To
update this repo with the latest from note-c:

```
git subtree pull --prefix=src/note-c --squash https://github.com/blues/note-c.git master
```

[blues]: https://blues.com
[notehub]: https://notehub.io
[note-c]: https://github.com/blues/note-c
[m5stack]: https://github.com/blues/m5stack
[archive]: https://github.com/blues/note-arduino/archive/master.zip


