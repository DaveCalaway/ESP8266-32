import os
import shutil
try:
    import configparser
except ImportError:
    import ConfigParser as configparser

config = configparser.ConfigParser()
config.read("platformio.ini")
version = config.get("env:esp32dev", "OTA_version")

f = open("firmware.json", "w")
f.write('{\n    "version": %s\n}' % (version))
f.close()

root = os.getcwd()
shutil.move(root + "/.pio/build/esp32dev/firmware.bin", "{}".format(root))