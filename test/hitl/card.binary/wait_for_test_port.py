Import("env")


def after_upload(source, target, env):
    port = env.GetProjectOption("test_port")
    print("waiting for " + port + " ...")
    import serial
    while True:
        try:
            s = serial.Serial(port)
            break
        except:
            pass


env.AddPostAction("upload", after_upload)
