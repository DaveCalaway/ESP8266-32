# MQTT TLS

This guide help you to use the TLS Protocol over MQTT.  


Tested over  | Broker | Version
------- |-------| -------
------- | Mosquitto |  2.5.5
------- | openssl |  1.1.1a
------- | MQTT.fx |  1.1.1a

## Basic

**Install Mosquitto:** https://goo.gl/2jn9yx  
On the Mosquitto we already have the openssl.

- **TLS basic:** [link](https://goo.gl/NsRjPW)  

- ...


## Useful stuff

Base guide: https://goo.gl/raSQrf  

**Move files from the RPi to the PC:**  
It's very useful command to move the ca.crt file, crated on the RPi, to the PC if you uses the MQTT.fx .  

Open a console on your PC:  

    scp /etc/mosquitto/ca_certificates/ca.crt .  


**Something not works?**  
See the log:  

    sudo cat /var/log/mosquitto/mosquitto.log 
