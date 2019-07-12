import os
import shutil
try:
    import configparser
except ImportError:
    import ConfigParser as configparser

Import("env")


#
# new firmware ready
#
def after_upload(source, target, env):
    config = configparser.ConfigParser()
    config.read("platformio.ini")
    version = config.get("env:esp32dev", "OTA_version")

    f = open("firmware.json", "w")
    f.write('{\n    "version": %s\n}' % (version))
    f.close()

    root = os.getcwd()
    try:
        os.remove(root + "/firmware.bin")
    except OSError:
        print "no .bin file"

    shutil.move(root + "/.pio/build/esp32dev/firmware.bin", "{}".format(root))

    print "Firmware and Json updated."



env.AddPostAction("upload", after_upload)
