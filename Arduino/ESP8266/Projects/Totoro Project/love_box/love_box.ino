/*
   This sketch is made for ESP01 with:
     - GPIO2 for button
     - GPIO1 for led ( internal )
     PinOut: https://goo.gl/CeR3Fk

     Instructables guide: 
     This sketch is for public domain. Davide Gariselli.
*/

// <--- ESP8266 + WiFi libraries --->
#include <ESP8266WiFi.h>

// <--- Include the correct MQTT Adafruit library --->
/*  ----> MQTT Adafruit library: https://goo.gl/4ewcc2
            Adafruit tutorial: https://goo.gl/BVXdso
    ----> MQTT Broker: io.adafruit.com
            Chrome: MQTTLens
*/
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// ---- MQTT connection ---------
// TODO: Configurable MQTT settings
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883  // use 8883 for SSL
#define AIO_USERNAME    "XXXX"
#define AIO_KEY         "XXX"

#define buttonPin 2 // GPIO2
#define led 1


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called for publishing.
Adafruit_MQTT_Publish love_box_push = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/love_box");

// Setup a feed called for subscribing to changes.
Adafruit_MQTT_Subscribe love_box_pull = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/love_box");


void setup() {

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  //Serial.begin(115200);
  delay(10);

  //Serial.println(F("Love box"));
  
  // ---------- How many Wifi do you have? ----------
#define Wifi_number 3
  // TODO: Configurable wifi settings
  char* myStrings[] = {"SSD", "pass", "SSD", "pass", "lov3b0x", "l0lloll0l",};

  // Connect to WiFi access point.
  for (int i = 0; i <= ((Wifi_number * 2) - 1); i = i + 2) {

    //        Serial.println();
    //        Serial.print("Connecting to ");
    //        Serial.println(myStrings[i]);
    //    Serial.print(myStrings[i + 1]);

    WiFi.begin(myStrings[i], myStrings[i + 1]);
    byte p = 0;
    while ( WiFi.status() != WL_CONNECTED ) {
      serial_dot();
      p++;
      if (p == 15) break;
    }
    if ( p < 15 )break;
  }

  // if connected blink 4 times
  for (int c = 0; c <= 4; c++ ) {
    digitalWrite(led, LOW);
    delay(300);
    digitalWrite(led, HIGH);
    delay(300);
  }
  //  Serial.println();
  //
  //  Serial.println("WiFi connected");
  //  Serial.print("Local IP address: ");
  //  Serial.println(WiFi.localIP());

  /*
     Setup MQTT subscription for command feed.
  */
  mqtt.subscribe(&love_box_pull);

}

void loop() {

  MQTT_connect();
  byte num = 0;
  byte buttonState = 1;

  buttonState = digitalRead(buttonPin);
  delay(15);
  //Serial.print("bottone: ");
  //Serial.println(buttonState);

  if (buttonState == LOW) {
    love_box_push.publish(1);
    //Serial.println(F("Spedito"));
    digitalWrite(led, LOW);
    delay(2000);
    digitalWrite(led, HIGH);
    // clear the channel
    love_box_push.publish(0);
  }

  // We'll use this to determine which subscription was received.
  // In this case we only have one subscription, so we don't really need it.
  // But if you have more than one its required so we keep it in here so you dont forget
  Adafruit_MQTT_Subscribe *subscription;

  //If the reader times out, the while loop will fail.
  //However, say we do get a valid non-zero return.
  // mqtt.readSubscription(timeInMilliseconds)
  while ((subscription = mqtt.readSubscription(500))) {
    // we only care about the love
    if (subscription == &love_box_pull) {
      //The message is in feedobject.lastread, convert string to int = atoi
      char *value = (char *)love_box_pull.lastread;
      //Serial.print(F("Received: "));
      int received = atoi(value);
      if ( received != 0 ) {
        //Serial.print(F("Comando"));
        num = received;
        //Serial.println(num);
      }
    }
  }

  // if i recived something, wait
  if ( num == 2 ) {
    while ( digitalRead(buttonPin) == HIGH ) {
      //Serial.println("aspetto bottone");
      yield();// https://goo.gl/PcdtfY
      // blink until you will press the button
      digitalWrite(led, LOW);
      delay(250);
      digitalWrite(led, HIGH);
      delay(250);
    }
    //Serial.println("reset");
    digitalWrite(led, HIGH);
    delay(2000);
  }
}


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  //Serial.println("Connecting to MQTT... ");

  //uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    //Serial.println(mqtt.connectErrorString(ret));
    //Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
  }
  //Serial.println("MQTT Connected!");
}

void serial_dot() {
  delay(1000);
  //Serial.print(".");
}
