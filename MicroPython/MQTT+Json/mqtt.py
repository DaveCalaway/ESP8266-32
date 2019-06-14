# DaveCalaway
# Connection to adafruit io with the json parameters
#
# Thx MikeTeachman of the base code: https://goo.gl/xytzpX
#

import network # Network basics
import time
import ujson
import machine # functions related to the hardware https://goo.gl/N2Qmki
import gc # control the garbage collector https://goo.gl/1SfFcx
from umqtt.simple import MQTTClient # https://goo.gl/vckpNx

#
# connect the ESP8266 to local wifi network
#
with open('document.json') as data_file:
    # pay attention, load NOT loads
    data = ujson.load(data_file)
data_file.close()
yourWifiSSID = data["wifi"]["SSD"]
yourWifiPassword = data["wifi"]["pass"]
#print('SSD: {} & pass: {}'.format(yourWifiSSID,yourWifiPassword))

sta_if = network.WLAN(network.STA_IF) # create station interface
sta_if.active(True) # activate the interface
sta_if.connect(yourWifiSSID, yourWifiPassword) # connect to an AP
while not sta_if.isconnected():
  pass
print("yeah")
#
# connect ESP8266 to Adafruit IO using MQTT
#
myMqttClient = data["ada"]["client"]  # can be anything unique
adafruitIoUrl = "io.adafruit.com"
adafruitUsername = data["ada"]["Username"]  # can be found at "My Account" at adafruit.com
adafruitAioKey = data["ada"]["AioKey"]  # can be found by clicking on "VIEW AIO KEYS" when viewing an Adafruit IO Feed
c = MQTTClient(myMqttClient, adafruitIoUrl, 0, adafruitUsername, adafruitAioKey)
c.connect()

# note on feed name in the MQTT Publish:
#    format:
#      "<adafruit-username>/feeds/<adafruitIO-feedname>"
#    example:
#      "MikeTeachman/feeds/feed-TempInDegC"
#
#
while True:
   c.publish('{}/feeds/love_box'.format(adafruitUsername), "esp")  # publish temperature to adafruit IO feed
   time.sleep(5)  # number of seconds between each Publish

c.disconnect()
