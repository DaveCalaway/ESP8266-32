#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPUpdate.h>

#define ledPin 2  // internal LED
// ==== OTA DEF ====
#define UPDATE_BIN_URL "https://github.com/DaveCalaway/ESP8266-32/blob/master/Arduino/ESP32/RTOS/OTA_HTTP/firmware.bin"
#define UPDATE_JSON_URL "https://github.com/DaveCalaway/ESP8266-32/blob/master/Arduino/ESP32/RTOS/OTA_HTTP/firmware.json"
String firwareVersion;
// =================

const size_t capacity = JSON_OBJECT_SIZE(1) + 30;
DynamicJsonDocument doc(capacity);

const char *ssid = "715C4L1";
const char *pass = "arbibbio123";

void OTAcheck(void *parameter);  // Function Declaration, now i can put the function on the bottom of the code

// --- Tasks ---
TaskHandle_t Task1;

//
// --- BOTH CYLINDER ---
//
void bothCylinder() {
    xTaskCreatePinnedToCore(
        OTAcheck,
        "Task_1",
        10000,
        NULL,
        1,
        &Task1,
        0);
}

//
// --- WIFI CON ---
//
void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WiFi.SSID());

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
// --- SETUP ---
//
void setup() {
    bothCylinder();

    pinMode(ledPin, OUTPUT);

    Serial.begin(115200);
}

//
//--- TASK1 ON CORE0 ---
// Check update task
void OTAcheck(void *parameter) {
    for (;;) {
        bool isNew = 0;
        String JSONpayload;
        WiFiClient client;
        HTTPClient http;

        // The line below is optional. It can be used to blink the LED on the board during flashing
        // The LED will be on during download of one buffer of data from the network. The LED will
        // be off during writing that buffer to flash
        // On a good connection the LED should flash regularly. On a bad connection the LED will be
        // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
        // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
        httpUpdate.setLedPin(ledPin, LOW);

        // downloading the json file with the last version
        http.begin(UPDATE_JSON_URL);  //HTTP
        int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK) {
            JSONpayload = http.getString();
            Serial.println(JSONpayload);
        }
        http.end();

        // check the software version
        DeserializationError error = deserializeJson(doc, JSONpayload);  // Deserialize the JSON document

        // Test if parsing succeeds.
        if (error) {
            Serial.print(F("                deserializeJson() failed: "));
            Serial.print(error.c_str());
        } else {
            double lastFirware = doc["version"];  // extract the version
            // is it new?
            if (firwareVersion.toFloat() > lastFirware) {
                firwareVersion = lastFirware;
                Serial.print("              New firmware available: ");
                Serial.println(lastFirware);
                isNew = 1;
            } else
                Serial.print("              No firmware available");
        }

        if (isNew) {
            t_httpUpdate_return ret = httpUpdate.update(client, UPDATE_BIN_URL);
            // Or:
            //t_httpUpdate_return ret = httpUpdate.update(client, "server", 80, "file.bin");

            switch (ret) {
                case HTTP_UPDATE_FAILED:
                    Serial.printf("              HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
                    break;

                case HTTP_UPDATE_NO_UPDATES:
                    Serial.println("              HTTP_UPDATE_NO_UPDATES");
                    break;

                case HTTP_UPDATE_OK:
                    Serial.println("              HTTP_UPDATE_OK");
                    break;
            }
        }

        delay(5000);
    }
}

//
// --- LOOP ON CORE1---
// -- WiFi --
//
void loop() {
    //Serial.printf("Loop on Core %d\n ", xPortGetCoreID());

    // check the WiFi
    digitalWrite(ledPin, HIGH);
    if (WiFi.status() != WL_CONNECTED) setup_wifi();  // if not connected
    digitalWrite(ledPin, LOW);

    delay(500);
}