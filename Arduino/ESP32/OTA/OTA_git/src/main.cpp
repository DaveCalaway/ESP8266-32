#include <Arduino.h>
#include <HTTPUpdate.h>
#include <PubSubClient.h>

#define ledPin 2  // build in
#define HTTP_OTA ""

//
// --- WIFI ---
//
const char *ssid = "AndroidAP_7251";
const char *pass = "culoculoculo";

//
// --- MQTT ---
//
const char *rootTopic = "/ota/";
const char *mqtt_server = "broker.mqttdashboard.com";  // http://www.hivemq.com/demos/websocket-client/
String clientIdRandom;

WiFiClient espClient;
PubSubClient client(espClient);

bool firstPub = 0;

//
// --- WIFI CON ---
//
void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(150);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

//
// --- OTA ---
//
void doOTA() {
    t_httpUpdate_return ret = httpUpdate.update(espClient, HTTP_OTA);
    // Or:
    //t_httpUpdate_return ret = httpUpdate.update(espClient, "server", 80, "file.bin");

    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            break;
    }
}

//
// --- RECON MQTT ---
//
void reconnectMqtt() {
    // Loop until we're reconnected
    while (!espClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(clientIdRandom.c_str())) {  // espClient connect to the broker
            Serial.println("connected.");

            client.subscribe(rootTopic);  // subscribe to the OTA channel
        }
    }
}

//
// --- CALLBACK ---
//
void callback(char *topic, byte *payload, unsigned int length) {
    for (byte i = 0; i < 3; i++) {  // blink 3 times
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
    }
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    char msg[length];

    for (int i = 0; i < length; i++) {  // print the payload
        msg[i] = payload[i];
        Serial.print((char)payload[i]);
    }
    Serial.println();

    int val = atoi(msg);  // char string to int

    if (strcmp(topic, rootTopic) == 0) {  // OTA check
        if (val) {
            Serial.println("OTA load");
            doOTA();
        } else
            Serial.println("no OTA requested");
    }
}

void setup() {
    Serial.begin(115200);  // Serial
    pinMode(ledPin, OUTPUT);

    clientIdRandom = String(random(0xffff), HEX);

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

//
// --- LOOP ON CORE1 ---
//
void loop() {
    // check the connections
    if (!client.connected()) {  // if not connected
        digitalWrite(ledPin, HIGH);
        if (WiFi.status() != WL_CONNECTED) setup_wifi();  // if not connected
        reconnectMqtt();
        digitalWrite(ledPin, LOW);
    }

    client.loop();

    if (firstPub == 0) {
        firstPub = 1;
        client.publish(rootTopic, "ESP32 OTA by MQTT");  // Once connected, publish an Announcement
    }

    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
}