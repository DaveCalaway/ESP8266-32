#include <Arduino.h>
#include <HTTPUpdate.h>
#include <PubSubClient.h>
#include <time.h>

#define ledPin 2  // build in
#define HTTPS_OTA "https://download.wetransfer.com//eu2/9ffd9f11cba3008a8cfef8818e579cc720190722093453/8b7c071a8cbaa19e9141794434b846bebb4808d7/Blink.ino.esp32.bin?cf=y&token=eyJhbGciOiJIUzI1NiJ9.eyJ1bmlxdWUiOiI5ZmZkOWYxMWNiYTMwMDhhOGNmZWY4ODE4ZTU3OWNjNzIwMTkwNzIyMDkzNDUzIiwiZmlsZW5hbWUiOiJCbGluay5pbm8uZXNwMzIuYmluIiwiZXhwaXJlcyI6MTU2Mzc4ODcxMywiaG90IjpmYWxzZSwiYnl0ZXNfZXN0aW1hdGVkIjoxOTcyMTYsImVudHJpZXNfZmluZ2VycHJpbnQiOiI4YjdjMDcxYThjYmFhMTllOTE0MTc5NDQzNGI4NDZiZWJiNDgwOGQ3Iiwid2F5YmlsbF91cmwiOiJodHRwOi8vcHJvZHVjdGlvbi5iYWNrZW5kLnNlcnZpY2UuZXUtd2VzdC0xLnd0OjkyOTIvd2F5YmlsbC92MS9mNDBiNWEyMzdjZThjZjNlYTFlODk5OTZlOTM5ZDFiNDZiYzc0NDY2MzdkNGIzZWEyOTMxYTFlYTYxYmFmYjgyZTRkZjA1ODRmNmVlM2Q4NDkwYTZiMzA0NmE1MWM0NmMwNTg4MTdmNDg3YjQxNTM1ZmRlNzM1YTdhZWIxYjQyNSIsImNhbGxiYWNrIjoie1wiZm9ybWRhdGFcIjp7XCJhY3Rpb25cIjpcImh0dHA6Ly9wcm9kdWN0aW9uLmZyb250ZW5kLnNlcnZpY2UuZXUtd2VzdC0xLnd0OjMwMDAvYXBpL2JhY2tlbmQvdHJhbnNmZXJzLzlmZmQ5ZjExY2JhMzAwOGE4Y2ZlZjg4MThlNTc5Y2M3MjAxOTA3MjIwOTM0NTMvZG93bmxvYWRzLzY4MDY1NzYxNjgvY29tcGxldGVkXCJ9LFwiZm9ybVwiOntcInN0YXR1c1wiOltcInBhcmFtXCIsXCJzdGF0dXNcIl0sXCJkb3dubG9hZF9pZFwiOlwiNjgwNjU3NjE2OFwifX0ifQ.-OQ1nRVY5AA-12e-i-FriaXzPvcWjLmY1QnvNiAWQd0"

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

/**
 * This is lets-encrypt-x3-cross-signed.pem
 * 10/19/2025 2:00:00 AM CEST
 */
const char *rootCACertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIESTCCAzGgAwIBAgITBn+UV4WH6Kx33rJTMlu8mYtWDTANBgkqhkiG9w0BAQsF\n"
    "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
    "b24gUm9vdCBDQSAxMB4XDTE1MTAyMjAwMDAwMFoXDTI1MTAxOTAwMDAwMFowRjEL\n"
    "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEVMBMGA1UECxMMU2VydmVyIENB\n"
    "IDFCMQ8wDQYDVQQDEwZBbWF6b24wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
    "AoIBAQDCThZn3c68asg3Wuw6MLAd5tES6BIoSMzoKcG5blPVo+sDORrMd4f2AbnZ\n"
    "cMzPa43j4wNxhplty6aUKk4T1qe9BOwKFjwK6zmxxLVYo7bHViXsPlJ6qOMpFge5\n"
    "blDP+18x+B26A0piiQOuPkfyDyeR4xQghfj66Yo19V+emU3nazfvpFA+ROz6WoVm\n"
    "B5x+F2pV8xeKNR7u6azDdU5YVX1TawprmxRC1+WsAYmz6qP+z8ArDITC2FMVy2fw\n"
    "0IjKOtEXc/VfmtTFch5+AfGYMGMqqvJ6LcXiAhqG5TI+Dr0RtM88k+8XUBCeQ8IG\n"
    "KuANaL7TiItKZYxK1MMuTJtV9IblAgMBAAGjggE7MIIBNzASBgNVHRMBAf8ECDAG\n"
    "AQH/AgEAMA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUWaRmBlKge5WSPKOUByeW\n"
    "dFv5PdAwHwYDVR0jBBgwFoAUhBjMhTTsvAyUlC4IWZzHshBOCggwewYIKwYBBQUH\n"
    "AQEEbzBtMC8GCCsGAQUFBzABhiNodHRwOi8vb2NzcC5yb290Y2ExLmFtYXpvbnRy\n"
    "dXN0LmNvbTA6BggrBgEFBQcwAoYuaHR0cDovL2NydC5yb290Y2ExLmFtYXpvbnRy\n"
    "dXN0LmNvbS9yb290Y2ExLmNlcjA/BgNVHR8EODA2MDSgMqAwhi5odHRwOi8vY3Js\n"
    "LnJvb3RjYTEuYW1hem9udHJ1c3QuY29tL3Jvb3RjYTEuY3JsMBMGA1UdIAQMMAow\n"
    "CAYGZ4EMAQIBMA0GCSqGSIb3DQEBCwUAA4IBAQCFkr41u3nPo4FCHOTjY3NTOVI1\n"
    "59Gt/a6ZiqyJEi+752+a1U5y6iAwYfmXss2lJwJFqMp2PphKg5625kXg8kP2CN5t\n"
    "6G7bMQcT8C8xDZNtYTd7WPD8UZiRKAJPBXa30/AbwuZe0GaFEQ8ugcYQgSn+IGBI\n"
    "8/LwhBNTZTUVEWuCUUBVV18YtbAiPq3yXqMB48Oz+ctBWuZSkbvkNodPLamkB2g1\n"
    "upRyzQ7qDn1X8nn8N8V7YJ6y68AtkHcNSRAnpTitxBKjtKPISLMVCx7i4hncxHZS\n"
    "yLyKQXhw2W2Xs0qLeC1etA+jTGDK4UfLeC0SF7FSi8o5LL21L8IzApar2pR/\n"
    "-----END CERTIFICATE-----\n";

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