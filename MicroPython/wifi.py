import network

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
if not wlan.isconnected():
    print('connecting to network...')
    wlan.connect('xxx', 'yyy')
    while not wlan.isconnected():
        pass
print('network config:', wlan.ifconfig())
