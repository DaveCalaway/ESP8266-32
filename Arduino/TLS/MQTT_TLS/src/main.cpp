/*
   Sketch made for the Wemos D1 Mini.
   Read external ADC (ADS1015) and create a Json file for the MQTT channel.
   Controls the onBorad LED by pubbling 0 or 1.

   - MQTT:
      - mqtt_rasp : the address of the raspberry with BrokerMosquitto
      - client.subscribe : inTopic ( default )
      - client.pub : outTopic ( default )

   - Rasp:
      - write this command to listen on the default channel:
            mosquitto_sub –d –t outTopic
      - write this command to write on the default channel:
            mosquitto_pub -t inTopic -m 1

   - ADS1015: all input signals to these devices must be between ground potential and VCC.
      - Defautl address: 0x48 with ADDR to GND
      - SDA/SCL default to pins 4 & 5 but any two pins can be assigned as
         SDA/SCL using Wire.begin(SDA,SCL)

   Davide Gariselli - Mobimentum 2019
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // MQTT Client
#include <ArduinoJson.h> // V5 stable
#include <Adafruit_ADS1015.h> // https://goo.gl/3tNhuP

int oldtime = 0;
int refrash = 5000;
char msg[50];
const char* wifi = "xxx";
const char* pass = "yyy";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_rasp = "192.168.1.70";

// ArduinoJson
DynamicJsonBuffer jsonBuffer(200);
JsonObject& root = jsonBuffer.createObject();
// MQTT
WiFiClient espClient;
PubSubClient client(espClient);
// ADC 12-bit
Adafruit_ADS1015 ads;


//
// ============== CA_CERT ==============
//
const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC1TCCAb2gAwIBAgIJAMPt1Ms37+hLMA0GCSqGSIb3DQEBCwUAMCExCzAJBgNV
BAYTAlVTMRIwEAYDVQQDDAkxMjcuMC4wLjMwHhcNMTgwMzE0MDQyMTU0WhcNMjkw
NTMxMDQyMTU0WjAhMQswCQYDVQQGEwJVUzESMBAGA1UEAwwJMTI3LjAuMC4zMIIB
IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxsa4qU/tlzN4YTcnn/I/ffsi
jOPc8QRcwClKzasIZNFEye4uThl+LGZWFIFb8X8Dc+xmmBaWlPJbqtphgFKStpar
DdduHSW1ud6Y1FVKxljo3UwCMrYm76Q/jNzXJvGs6Z1MDNsVZzGJaoqit2H2Hkvk
y+7kk3YbEDlcyVsLOw0zCKL4cd2DSNDyhIZxWo2a8Qn5IdjWAYtsTnW6MvLk/ya4
abNeRfSZwi+r37rqi9CIs++NpL5ynqkKKEMrbeLactWgHbWrZeaMyLpuUEL2GF+w
MRaAwaj7ERwT5gFJRqYwj6bbfIdx5PC7h7ucbyp272MbrDa6WNBCMwQO222t4wID
AQABoxAwDjAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCmXfrC42nW
IpL3JDkB8YlB2QUvD9JdMp98xxo33+xE69Gov0e6984F1Gluao0p6sS7KF+q3YLS
4hjnzuGzF9GJMimIB7NMQ20yXKfKpmKJ7YugMaKTDWDhHn5679mKVbLSQxHCUMEe
tEnMT93/UaDbWBjV6zu876q5vjPMYgDHODqO295ySaA71UkijaCn6UwKUT49286T
V9ZtzgabNGHXfklHgUPWoShyze+G3g29I1BR0qABoJI63zaNu8ua42v5g1RldxsW
X8yKI14mFOGxuvcygG8L2xxysW7Zq+9g+O7gW0Pm6RDYnUQmIwY83h1KFCtYCJdS
2PgozwkkUNyP
-----END CERTIFICATE-----
)EOF";



//
// ============== WiFi SETUP ==============
//
void setup_wifi() {
        Serial.println();

        WiFi.begin(wifi, pass);

        Serial.print("Connecting");
        while (WiFi.status() != WL_CONNECTED)
        {
                delay(500);
                Serial.print(".");
        }
        Serial.println();

        Serial.print("Connected, IP address: ");
        Serial.println(WiFi.localIP());
}



//
// ============== CALLBACK ==============
//
void callback(char* topic, byte* payload, unsigned int length) {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        for (int i = 0; i < length; i++) {
                Serial.print((char)payload[i]);
        }
        Serial.println();

        // Switch on the LED if an 1 was received as first character
        if ((char)payload[0] == '1') {
                digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
                // but actually the LED is on; this is because
                // it is acive low on the ESP-01)
        } else {
                digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
        }

}


//
// ============== RECONNECT ==============
//
void reconnect() {
        // Loop until we're reconnected
        while (!client.connected()) {
                Serial.print("Attempting MQTT connection...");
                // Attempt to connect
                if (client.connect("ESP8266Client")) {
                        Serial.println("connected");
                        // Once connected, publish an announcement...
                        client.publish("outTopic", "hello world");
                        // ... and resubscribe
                        client.subscribe("inTopic");
                } else {
                        Serial.print("failed, rc=");
                        Serial.print(client.state());
                        Serial.println(" try again in 5 seconds");
                        // Wait 5 seconds before retrying
                        delay(5000);
                }
        }
}



//
// ============== SETUP ==============
//
void setup() {
        Serial.begin(9600);

        // WIFI connection
        setup_wifi();

        // The ADC input range (or gain) can be changed via the following
        // functions, but be careful never to exceed VDD +0.3V max, or to
        // exceed the upper and lower limits if you adjust the input range!
        // Setting these values incorrectly may destroy your ADC!
        //                                                                ADS1015  ADS1115
        //                                                                -------  -------
        // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
        // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
        // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
        // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
        // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
        // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
        ads.begin();

        // MQTT TLS
        BearSSL::WiFiClientSecure client;
        BearSSL::X509List cert(ca_cert);
        client.setTrustAnchors(&cert);

        // MQTT
        client.setServer(mqtt_rasp, 1883);
        client.setCallback(callback);
}


//
// ============== LOOP ==============
//
void loop() {

        // Wait for MQTT client
        if (!client.connected()) {
                reconnect();
        }
        client.loop();


        if ( millis() - oldtime > refrash ) {
                oldtime = millis();

                // Read from ADC
                int16_t adc0 = ads.readADC_SingleEnded(0);

                // Add values in the object
                root["sensor"] = "adc";
                root["value"] = adc0;

                Serial.print("AIN0: "); Serial.println(adc0);

                // print Json in nice format
                root.prettyPrintTo(Serial);
                root.printTo(msg);

                // MQTT Public
                //snprintf (msg, 75, "hello world #%ld", value); // ( char * s, size_t n, const char * format, ... )
                Serial.print("Publish message: ");
                //Serial.println(msg);
                client.publish("outTopic", msg); //publish (topic, payload) - https://goo.gl/eR5BWA
        }
}
