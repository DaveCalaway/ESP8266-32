# Blink the internal led
from machine import Pin
import time

p0 = Pin(2, Pin.OUT)    # create output pin on GPIO2

while True:
    p0.on()                 # set pin to "on" (high) level
    time.sleep(1)           # sleep 1s
    p0.off()                # set pin to "off" (low) level
    time.sleep(1)           # sleep 1s
