/*
   Create a esp32BLE Server with a New Service and controls the LED_BUILTIN with pwm signal sends by BLE.

   ESP32_BLE_Arduino: https://goo.gl/3LxyDD
   ESP32_BLE_Arduino guide: https://goo.gl/BWU9B8
*/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// ============ DEBUG ============
#define DEBUG
#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.print (x)
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_PRINT_HEX(x)  Serial.print (x, HEX)
 #define DEBUG_PRINT_DEC(x)  Serial.print (x, DEC)
 #define DEBUG_flush()   Serial.flush ()
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINT_HEX(x)
 #define DEBUG_PRINT_DEC(x)
 #define DEBUG_PRINTLN(x)
 #define DEBUG_flush()
#endif
// ===============================

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// ====== PWM ======
#define LEDC_CHANNEL_0     0 // use first channel of 16 channels (started from zero)
#define LEDC_TIMER_13_BIT  13 // use 12 bit precission for LEDC timer
#define LEDC_BASE_FREQ     5000 // use 5000 Hz as a LEDC base frequency
#define LED_PIN            26 // LED_BUILTIN = 2

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
byte pwm = 0; // from 0 to 255


class MyCallbacks : public BLECharacteristicCallbacks {
void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        pwm = atoi(value.c_str()); // string to int

#ifdef DEBUG
        if (value.length() > 0) {
                Serial.println("*********");
                Serial.print("New value: ");
                for (int i = 0; i < value.length(); i++)
                        Serial.print(value[i]);

                Serial.println();
                Serial.println("*********");
        }
#endif
}
};


// Arduino like analogWrite
// value has to be between 0 and valueMax
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
        // calculate duty, 8191 from 2 ^ 13 - 1
        uint32_t duty = (8191 / valueMax) * min(value, valueMax);

        // write duty to LEDC
        ledcWrite(channel, duty);
}


void setup() {
#ifdef DEBUG
        Serial.begin(115200);
#endif

        // Analog write
        ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
        ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);

        BLEDevice::init("Dress Coders");
        BLEServer *pServer = BLEDevice::createServer();

        BLEService *pService = pServer->createService(SERVICE_UUID);

        BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                CHARACTERISTIC_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE
                );

        pCharacteristic->setCallbacks(new MyCallbacks());

        pCharacteristic->setValue("PWM test.");
        pService->start();

        BLEAdvertising *pAdvertising = pServer->getAdvertising();
        pAdvertising->start();
}

void loop() {
        if(pwm > 0) {
                // set the brightness on LEDC channel 0
                ledcAnalogWrite(LEDC_CHANNEL_0, brightness);

                // change the brightness for next time through the loop:
                brightness = brightness + fadeAmount;

                // reverse the direction of the fading at the ends of the fade:
                if (brightness <= 0 || brightness >= pwm) {
                        fadeAmount = -fadeAmount;
                }
                // wait for 30 milliseconds to see the dimming effect
                delay(30);
        }
        else ledcAnalogWrite(LEDC_CHANNEL_0, 0);
}
