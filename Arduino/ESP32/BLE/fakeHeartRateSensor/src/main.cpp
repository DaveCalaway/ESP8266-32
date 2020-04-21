/*
   Create a esp32BLE server with "Heart Rate Measurement" Service.
   When the Server(esp32) is connected to a Client(Phone), will sends periodic notifications, Server CallBack.
   When the Client is disconnect, the Server stops.

   See the following for generating UUIDs:
   https://www.uuidgenerator.net/
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service -> Heart Rate 0x180D
   3. Create a BLE Characteristic on the Service -> HeartRateMeasurement 0x2A37
   4. Create a BLE Descriptor on the characteristic -> Notify 0x2902
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.

   ESP32_BLE_Arduino: https://goo.gl/3LxyDD
   ESP32_BLE_Arduino guide: https://goo.gl/BWU9B8
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//BLEServer* pServer = NULL;
//BLECharacteristic* pHeartRateMeasurement = NULL; // Heart Rate Measurement
bool deviceConnected = false;
bool oldDeviceConnected = false;
byte heart[8] = { 0b00001110, 60, 0, 0, 0, 0, 0, 0};
byte bpm;

#define ServerName "Fake Heart Rate Sensor"

// ======== Services: Heart Rate ========
//#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // GATT Services: Generic https://goo.gl/JV7QxP
//#define SERVICE_UUID        "0000180D-0000-1000-8000-00805f9b34fb" // GATT Services: Heart Rate 0x180D
#define heartRateService BLEUUID((uint16_t)0x180D)

//#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // GATT Characteristics: Generic https://goo.gl/6zBgHb
//#define CHARACTERISTIC_UUID "00002A37-0000-1000-8000-00805f9b34fb" // GATT Characteristic: Heart Rate Measurement 0x2A37
BLECharacteristic heartRateMeasurementCharacteristics(BLEUUID((uint16_t)0x2A37), BLECharacteristic::PROPERTY_NOTIFY);

// Descriptor
BLEDescriptor heartRateDescriptor(BLEUUID((uint16_t)0x2901)); // Characteristic User Description https://goo.gl/AJGx7F
// ========================================


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



// ============= Server CALBACK =============
class MyServerCallbacks : public BLEServerCallbacks {
void onConnect(BLEServer* pServer) {
        deviceConnected = true;
};

void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
}
};


// ============= INIT BLE =============
void InitBLE(){
        // Create the BLE Device
        BLEDevice::init(ServerName);

        // Create the BLE Server
        //pServer = BLEDevice::createServer();
        BLEServer *pServer = BLEDevice::createServer();

        // Create the Server CallBack
        pServer->setCallbacks(new MyServerCallbacks());

// ==================== Create the BLE Service ====================
        BLEService *pHeart = pServer->createService(heartRateService); // Heart Rate

        // Create a BLE Heart Rate Measurement  Characteristic
        // pHeartRateMeasurement = pHeart->createCharacteristic(
        //         CHARACTERISTIC_UUID, // Heart Rate Measurement
        //         BLECharacteristic::PROPERTY_READ   |
        //         BLECharacteristic::PROPERTY_WRITE  |
        //         BLECharacteristic::PROPERTY_NOTIFY |
        //         BLECharacteristic::PROPERTY_INDICATE
        //         );

        // Add BLE Heart Rate Measurement Characteristic
        pHeart->addCharacteristic(&heartRateMeasurementCharacteristics); // Heart Rate Measurement

        // Create a BLE Descriptor
        heartRateDescriptor.setValue("Rate from 0 to 200"); // 0x2901 https://goo.gl/AJGx7F
        // Set the Descriptor to a specific Characteristic
        heartRateMeasurementCharacteristics.addDescriptor(&heartRateDescriptor);

        // BLE notification for heartRateMeasurementCharacteristics
        heartRateMeasurementCharacteristics.addDescriptor(new BLE2902()); // https://goo.gl/BmsvWu
// ========================================

        // Advertises the Device that the Server has a Heart Rate Service 0x180D
        pServer->getAdvertising()->addServiceUUID(heartRateService);

        // Start the service
        pHeart->start();

        // Start advertising
        pServer->getAdvertising()->start();
        // BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        // pAdvertising->addServiceUUID(SERVICE_UUID);
        // pAdvertising->setScanResponse(false);
        // pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
        // BLEDevice::startAdvertising();
        // DEBUG_PRINTLN("Waiting a client connection to notify...");
}



// ============= SETUP =============
void setup() {
  #ifdef DEBUG
        Serial.begin(115200);
        Serial.println("Start");
  #endif

        InitBLE();
}


// ============= LOOP =============
void loop() {
        // notify changed value
        if (deviceConnected) {
                heart[1] = (byte)bpm;
                int energyUsed = 3000;
                heart[3] = energyUsed / 256;
                heart[2] = energyUsed - (heart[2] * 256);
                Serial.println(bpm);

                // Feed the characteristic
                heartRateMeasurementCharacteristics.setValue(heart, 8);
                // Send a notification
                heartRateMeasurementCharacteristics.notify();

                bpm++;

                delay(500);
                //delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
        }
        // disconnecting
        if (!deviceConnected && oldDeviceConnected) {
                delay(500); // give the bluetooth stack the chance to get things ready
                //pServer->startAdvertising(); // restart advertising
                //pServer->getAdvertising()->start(); // restart advertising
                DEBUG_PRINTLN("start advertising");
                oldDeviceConnected = deviceConnected;
        }
        // connecting
        if (deviceConnected && !oldDeviceConnected) {
                // do stuff here on connecting
                oldDeviceConnected = deviceConnected;
        }
}
