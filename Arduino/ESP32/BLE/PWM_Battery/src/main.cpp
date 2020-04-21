/*
   Create a esp32BLE Server with Battery and PWM Services.
   When the Server(esp32) is connected to a Client(Phone), the Client can request the Battery level and
   write the PWM.


   ESP32_BLE_Arduino: https://goo.gl/3LxyDD
   ESP32_BLE_Arduino guide: https://goo.gl/BWU9B8

*/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#define ServerName "PWM+battery"
#define analogPin 36


// ====== PWM ======
#define LEDC_CHANNEL_0     0 // use first channel of 16 channels (started from zero)
#define LEDC_TIMER_13_BIT  13 // use 12 bit precission for LEDC timer
#define LEDC_BASE_FREQ     5000 // use 5000 Hz as a LEDC base frequency
#define LED_PIN            26 // LED_BUILTIN = 2


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


byte pwm = 0; // from 0 to 255
byte brightness = 0;
byte steps = 5; // steps for the PWM
bool battery_Read = 0;

// ======== New Services: PWM Service ========
// PWM Service
#define pwmService "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define PWM_CHARACTERISTIC "beb5483e-36e1-4688-b7f5-ea07361b26a8"
// Characteristic PWM
BLECharacteristic pwmCharacteristics(PWM_CHARACTERISTIC, BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor pwmDescriptor(BLEUUID((uint16_t)0x2901));
// ========================================

// ======== Services: Battery Service ========
// Service Battery Service https://goo.gl/HN4yL6
#define batteryService BLEUUID((uint16_t)0x180F)
// Characteristic Battery Level https://goo.gl/sek12v
BLECharacteristic batteryLevelCharacteristics(BLEUUID((uint16_t)0x2A19),BLECharacteristic::PROPERTY_READ);
// Characteristic User Description https://goo.gl/AJGx7F
BLEDescriptor batteryLevelDescriptor(BLEUUID((uint16_t)0x2901));
// ========================================



// ============= Characteristic CALLBACK =============
class PWMCallbacks : public BLECharacteristicCallbacks {
// Client write
void onWrite(BLECharacteristic* pwmCharacteristics) {
        std::string value = pwmCharacteristics->getValue();
        pwm = atoi(value.c_str()); // string to int

#ifdef DEBUG
        if (value.length() > 0) {
                Serial.print("PWM value: ");
                for (int i = 0; i < value.length(); i++)
                        Serial.println(value[i]);
        }
#endif
}
};

// Client read
class BatteryCallbacks : public BLECharacteristicCallbacks {
void onRead(BLECharacteristic* batteryLevelCharacteristics) {
        // Battery Measurement
        battery_Read = 1;
}

};


// ============= INIT BLE =============
void InitBLE(){
        // Create the BLE Device
        BLEDevice::init(ServerName);

        // Create the BLE Server
        BLEServer *pServer = BLEDevice::createServer();

// ============= Create the BLE Service =============
        BLEService *pPWM = pServer->createService(pwmService); // Heart Rate

        // Add BLE Heart Rate Measurement Characteristic
        pPWM->addCharacteristic(&pwmCharacteristics); // Heart Rate Measurement

        // Create a BLE Descriptor
        pwmDescriptor.setValue("PWM from 0 to 255"); // 0x2901 https://goo.gl/AJGx7F
        // Set the Descriptor to a specific Characteristic
        pwmCharacteristics.addDescriptor(&pwmDescriptor);

        // BLE notification for heartRateMeasurementCharacteristics
        // nop
// ========================================

// ============= Create the BLE Service =============
        BLEService *pBattery = pServer->createService(batteryService);

        // Add BLE Battery Level Characteristic
        pBattery->addCharacteristic(&batteryLevelCharacteristics);

        // Create a BLE Descriptor
        batteryLevelDescriptor.setValue("Battery Level %"); // 0x2901 https://goo.gl/AJGx7F
        // Set the Descriptor to a specific Characteristic
        batteryLevelCharacteristics.addDescriptor(&batteryLevelDescriptor);

        // BLE notification for heartRateMeasurementCharacteristics
        // nop
// ========================================


        // Set Callback
        batteryLevelCharacteristics.setCallbacks(new BatteryCallbacks());
        pwmCharacteristics.setCallbacks(new PWMCallbacks());

        // Advertises the Device that the Server
        pServer->getAdvertising()->addServiceUUID(pwmService);
        pServer->getAdvertising()->addServiceUUID(batteryService);

        // Start the service
        pPWM->start();
        pBattery->start();

        // Start advertising
        pServer->getAdvertising()->start();
        // BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        // pAdvertising->addServiceUUID(SERVICE_UUID);
        // pAdvertising->setScanResponse(false);
        // pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
        // BLEDevice::startAdvertising();
        // DEBUG_PRINTLN("Waiting a client connection to notify...");
}



// Arduino like analogWrite
// value has to be between 0 and valueMax
void ledcAnalogWrite(uint8_t channel, int value) {
        // calculate duty, 8191 from 2 ^ 13 - 1
        uint32_t duty = (8191 / 255) * min(value, 255);

        // write duty to LEDC
        ledcWrite(channel, duty);
}



// ============= SETUP =============
void setup() {
  #ifdef DEBUG
        Serial.begin(115200);
        Serial.println("Start");
  #endif

        // Analog Read
        pinMode(analogPin,INPUT);

        // Analog Write
        ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
        ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);

        InitBLE();
}


// ============= LOOP =============
void loop() {
        // Drive PWM
        if(pwm > 0) {
                // set the brightness on LEDC channel 0
                ledcAnalogWrite(LEDC_CHANNEL_0, brightness);

                // change the brightness for next time through the loop:
                brightness = brightness + steps;

                // reverse the direction of the fading at the ends of the fade:
                if (brightness <= 0 || brightness >= pwm) {
                        steps = -steps;
                }
                // wait for 30 milliseconds to see the dimming effect
                delay(30);
        }
        else ledcAnalogWrite(LEDC_CHANNEL_0, 0);

        // Battery Measurement
        if(battery_Read) {
                //int battery = analogRead(analogPin);
                int battery = 50;
                DEBUG_PRINT("Battery: ");
                DEBUG_PRINTLN(battery);
                // Feed the characteristic
                batteryLevelCharacteristics.setValue((uint8_t*)&battery, 1);
                battery_Read = 0;
        }
}
