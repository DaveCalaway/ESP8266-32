# DaveCalaway
#
# Recive from MQTT channel the variabiles needed to:
# -
#
# Broker:
#       mosquitto_sub -t sensors/temperature
#       mosquitto_pub -t sensors/temperature -m "lol"
#


import network  # Network basics
import time
import ujson
import machine  # functions related to the hardware https://goo.gl/N2Qmki
import gc  # control the garbage collector https://goo.gl/1SfFcx
from umqtt.simple import MQTTClient  # https://goo.gl/vckpNx


TOPIC = "sensors/temperature"
global data
global sta_if
global client


#
# load the json attributes
#
def json_load():
    global data
    # read standard json configuration
    with open('document.json') as data_file:
        # pay attention, load NOT loads
        data = ujson.load(data_file)
    data_file.close()
    print("json loaded.")



#
# no connection? enable WebRepl
#
def do_WebREPL():
    webrepl.start()
    # now you can send a new "document.json" with the corrects paramiters.


#
#  MQTT callback
#
def sub_cb(topic, msg):
    global TOPIC
    print("Message from topic: {} & message {}".format(topic, msg))

    # overrwrite old Json
    # with open('document.json', 'w') as data_file:
    #     data_file.write(message)
    # data_file.close()
    #
    # sta_if.active(False)


#
# connect the board to the MQTT Broker
#
def do_mqtt():
    global data
    global client
    host = data["mqtt"]["host"]
    #port = data["mqtt"]["port"]
    CLIENT_ID = ubinascii.hexlify(machine.unique_id())

    client = MQTTClient(CLIENT_ID,host)
    client.connect()
    client.set_callback(sub_cb)
    client.publish(TOPIC, "ESP8266 uPython ready")
    client.subscribe(TOPIC)



#
# connect the board
#
def do_connection():
    global data
    global sta_if
    yourWifiSSID = data["wifi"]["SSD"]
    yourWifiPassword = data["wifi"]["pass"]
    print('SSD: {} & pass: {}'.format(yourWifiSSID,yourWifiPassword))
    # connect the ESP8266 to local wifi network
    sta_if = network.WLAN(network.STA_IF) # create station interface
    sta_if.active(True) # activate the interface
    sta_if.connect(yourWifiSSID, yourWifiPassword)  # connect to an AP
    while not sta_if.isconnected():
        time.sleep_ms(100)
        pass
        #i=i+1
        #if(i > 3):
            #print("open WebRepl")
            #do_WebREPL()
            #break


    print("connected.")
    #connection to the MQTT Broker
    do_mqtt()



# --------------------------- MAIN ---------------------------

json_load()
do_connection()

while True:
    global sta_if
    global client
    # the device is connected?
    if (sta_if.active() and sta_if.isconnected()):
        client.check_msg()
        time.sleep(1)
        # run program
        pass

    else:
        do_connection()
