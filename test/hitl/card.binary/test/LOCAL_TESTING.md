The easiest way to run the end to end tests locally is to use the workflow file. However, if you need to get in even closer, then manual setup is the way to go.

The md5 server and localtunnel are long running servers so you'll need a few shell terminals open.

1. launch the md5 server

```bash
   cd test/hitl/scripts
   python3 -m md5srv.py --token <any-string> --save --dir test_files
```

2. Install localtunnel if you don't already have it `npm install -g localtunnel`

3. Launch localtunnel using your own subdomain (e.g. bobscobsjobs), so it doesn't keep changing. Localtunnel can crash when the server port goes away, so may need to be relauched.

```bash
   npx localtunnel -p 8080 -s <mysubdomain>
```

4. Create a proxy route in Notehub with a suitable name and a unique alias, such as "cobstest". The url should be the same as the output from localtunnel. Add an additional header `X-Access-Token` with the value of your token that was passed to the md5srv above.

5. Edit `test/hitl/card.binary/platformio.ini` and add these lines:
```
  build_flags =
     ...
	'-D NOTEHUB_PROXY_ROUTE_ALIAS="cobstest"'
	'-D PRODUCT_UID="<your product UID>"'
```

6. Edit `test/hitl/card.binary/test/test_card_binary.cpp` in VSCode. To run just selected tests you can comment out any macros in the `testsuite_card_binary()` function or add a filter. Commenting out functions doesn't work because they are then unused, causing a compiler error. Use `if (false)` instead.

7. Select the `env:debug` environment in the status bar at the bottom. Compile and upload the code using `>PlatformIO upload`, or press F5 to start a debugging session.

> Note that there is a bug in PlatformIO that uploads the code even if the build fails. I typically keep the test hardware switched off on the switched USB hub until the build is successful.

On pressing F5, a successful build looks like

```
Processing debug (platform: ststm32; board: bw_swan_r5; framework: arduino)
---------------------------------------------------------------------------------------------
Building...
 *  Terminal will be reused by tasks, press any key to close it.
```

8. Run the tests with
```bash
pio test -e debug --without-building --without-uploading --json-output-path test.json --junit-output-path test.xml
```

To see the notecard debug output, add `-vv` after `pio test`.