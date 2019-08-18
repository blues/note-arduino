# [Blues Wireless][blues]

The note-arduino Arduino library for communicating with Blues Wireless Notecard via serial or I²C. Includes example sketches in the [examples directory](examples).

This library allows you to control a Notecard by writing an Arduino sketch in C or C++.
Your sketch may programmatically configure Notecard and send Notes to [Notehub.io][notehub].

This library is a wrapper around the [note-c library][note-c], which it includes as a git submodule.

This library is used by the [m5stack repo][m5stack] m5note Arduino sketch, which requires it to be installed per the instructions below.

## Installation

1. Clone this library and its submodule from github into your Arduino libraries folder:
   ```
   git clone --recursive git@github.com:blues/note-arduino.git [PATH_TO_YOUR_ARDUINO_HOME_FOLDER]/libraries/note-arduino`
   ```
2. Start the Arduino IDE
3. Verify that the following items are available:
   ```
   File -> Examples -> Examples from Custom Libraries -> Blues Wireless Notecard -> 01-Requests
   Sketch -> Include library -> Contributed Libraries -> Blues Wireless Notecard
   ```

[blues]: https://blues.com
[notehub]: https://notehub.io
[note-c]: https://github.com/blues/note-c
[m5stack]: https://github.com/blues/m5stack
