# After PlatformIO's upload step (i.e. flashing the firmware onto the Swan), it
# runs the after_upload method registered in this file.

import os
import time

Import("env")


# There's an issue with PlatformIO where, after uploading, it expects to see the
# test port (i.e. the Swan's USB, which outputs test logging) too quickly. After
# upload, it may take a second for the device (/tmp/ns_mcu_usb) to appear, and
# so we don't want to proceed past the upload phase until it's appeared.
def _wait_for_test_port(env):
    port = env.GetProjectOption("test_port")
    print("Waiting for test port: " + port)

    timeout_secs = 10
    start = time.time()
    while not os.path.exists(port):
        if (time.time() - start) > timeout_secs:
            raise Exception(f"Timed out waiting for test port: {port}")
        time.sleep(0.1)

    return port


# The test firmware doesn't kick off until it receives the string "start\n" or
# "start\r\n" on the Swan's USB port.
def _start_tests(port):
    print("Sending 'start\\n' to Swan to kick off tests...")
    with open(port, "wb", buffering=0) as f:
        f.write(b"start\n")


def after_upload(source, target, env):
    port = _wait_for_test_port(env)
    _start_tests(port)

    # This pause seems to prevent this error from happening at the start of the
    # test step: "device reports readiness to read but returned no data (device
    # disconnected or multiple access # on port?)" Not totally sure why, but
    # maybe this gives the OS a chance to cleanup anything using the test port.
    time.sleep(3)


env.AddPostAction("upload", after_upload)
