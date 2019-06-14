# MQTT TLS Basic

Base guide: https://goo.gl/k5RV7k  

The guide is make for the Raspberry, but you can do the same steps over all OS.  
Probably is you change the OS, you will need to check the correct path.  

#### MacOS
Some problem, OSX? https://goo.gl/2FhbbW  
The directory */usr/local/etc/mosquitto/mosquitto.conf* is a link to the real directory */usr/local/Cellar/mosquitto/1.5.5/etc/mosquitto* .  

The Mosquitto execuble is at:  

    /usr/local/Cellar/mosquitto/1.5.5/sbin  

You can make changes to the configuration by editing:  

    /usr/local/etc/mosquitto/mosquitto.conf  

Start/stop or restart mosquitto:    

    brew services start mosquitto  
    brew services stop mosquitto  
    brew services restart mosquitto    


### TLS basic

On the Raspberry Pi, the Mosquitto is in the:  

    /etc/mosquitto  


**Client Requirements:**  
- CA (certificate authority) certificate of the CA that has signed the server certificate on the Mosquitto Broker.  

**Broker Requirements:**  
- CA certificate of the CA that has signed the server certificate on the Mosquitto Broker.  
- CA certificated server certificate.  
- Server Private key for decryption.  


### Creating and Installing Broker Certificates and keys

To create these certificates and keys we use the openssl software.  

1. Create a CA key pair:  

        openssl genrsa -des3 -out ca.key 2048   

2. Create CA certificate and use the CA key from step 1 to sign it:

        openssl req -new -x509 -days 1826 -key ca.key -out ca.crt  

3. Create a broker key pair don’t password protect:  

        openssl genrsa -out server.key 2048  

4. Create a broker certificate request using key from step 3:  

        openssl req -new -out server.csr -key server.key  

5. Use the CA certificate to sign the broker certificate request from step 4:  

        openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 360  

Now we should have:
- CA key file  
- CA certificate file  
- broker key file  
- broker certificate file  

We don’t need to copy the CA.key file. This file is used when creating new server or client certificates.  

### Move Certificates

On the RPi we have 2 folders: **certs** and **ca_certificates** .  
If you have not one or both folders, create them:

    mkdir certs  
    mkdir ca_certificates  

Move the file ca.crt under:    

    mv ca.crt /etc/mosquitto/ca_certificates  

Move the files server.crt and server.key under:  

    mv server.crt /etc/mosquitto/certs  
    mv server.key /etc/mosquitto/certs  


### Edit the Mosquitto conf file

Mosquitto loads the parameters on the "mosquitto.conf" at the mosquitto folder.  
If you have another "XYZ.conf" inside the "conf.d" folder, Mosquitto loads it after the primary "mosquitto.conf".  
So create a new one, called "tls.conf" and place it on the "conf.d":  

    cd /etc/mosquitto/conf.d  
    touch tls.conf  

Now edit the new tls.conf file, open it with the command:  

    sudo nano tls.conf  

so copy the list of commands below:  

    port 8883   
    cafile /etc/mosquitto/certs/ca.crt  
    keyfile /etc/mosquitto/certs/server.key  
    certfile /etc/mosquitto/certs/server.crt  
    tls_version tlsv1  

Save it and exit.  

### Test Mosquitto

We use two console, the Broker console is under the RPi, the Client is another PC.  
By the way, you can open two console over the RPi and use one for the Broker and the other for the Client.  

First stop Mosquitto:  

    sudo systemctl stop mosquitto

Now we need to run the Mosquitto Broker, in verbose mode, with the specific configuration:    

    mosquitto -c /etc/mosquitto/conf.d/mosquitto.conf -v   

The -v is the verbose mode, now you can see everything will arrive at the Broker.  

Open another console, for the Client, and type:  

- ~if you have the Broker and Client in the same Device:~  

      mosquitto_pub -h localhost -p 8883 --cafile ca.crt -t topic -m "hello" -d  

**I will investigate why it doesn't works.**  

- if you have the Broker on the RPi and the Client in another Device:  

Open MQTT.fx, click over the gear:  

![MQTT.fx](https://raw.githubusercontent.com/DaveCalaway/ESP8266/master/Arduino/TLS/MQTT_Broker.png)  

Edit the "broker address" with your RPi ip Address.  
Load the "ca.srt" file at specific folder.  

Click ok and type "hello" over the MQTT.fx and click Publish.   
See the RPi console and to check is a message is arrived.  


#### Something not works?
See the log:  

    sudo cat /var/log/mosquitto/mosquitto.log  
