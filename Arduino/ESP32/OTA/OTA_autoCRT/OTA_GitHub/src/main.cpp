#include <Arduino.h>
#include <HTTPUpdate.h>
#include <PubSubClient.h>
#include <time.h>

#define ledPin 2  // build in
#define HTTPS_OTA "https://raw.githubusercontent.com/DaveCalaway/ESP8266-32/master/Arduino/ESP32/OTA/Blink.ino.esp32.bin"

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
bool ota = 0;

//
// --- CLOCK ---
//
void setClock() {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC

    Serial.print(F("Waiting for NTP time sync: "));
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        yield();
        delay(500);
        Serial.print(F("."));
        now = time(nullptr);
    }

    Serial.println(F(""));
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}

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
// --- LOAD CERTIFICATE ---
//
void loadCertificate(String &payload) {
    HTTPClient http;

    http.begin("https://raw.githubusercontent.com/DaveCalaway/ESP8266-32/master/Arduino/ESP32/OTA/OTA_autoCRT/Certificate.txt");  // Certificate URL
    int httpCode = http.GET();

    if (httpCode > 0) {  // Check for the returning code
        payload = http.getString();

        int firstClosingBracket = payload.indexOf('<');  // where is <

        payload.remove(firstClosingBracket - 1);  // remove the data info

        // Serial.println(httpCode);
        // Serial.println(payload);
    }

    else {
        Serial.println("Error on HTTP request");
    }

    http.end();  // Free the resources
}

//
// --- OTA ---
//
void doOTA() {
    String payload;

    loadCertificate(payload);
    const char *rootCACertificate = payload.c_str();

    setClock();

    WiFiClientSecure espClient;
    espClient.setCACert(rootCACertificate);

    // Reading data over SSL may be slow, use an adequate timeout
    espClient.setTimeout(12000);

    t_httpUpdate_return ret = httpUpdate.update(espClient, HTTPS_OTA);
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
            Serial.println("OTA request");
            ota = 1;
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

    if (ota) doOTA();
    ota = 0;

    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
}