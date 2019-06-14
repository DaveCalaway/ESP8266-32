/*WiFi_Client Example
ESP in Client Mode: https://goo.gl/sCKRTR
The Client class creates clients that can access services 
provided by servers in order to send, receive and process data.

GitHub DaveCalaway
This example code is in the public domain.
*/
#include <ESP8266WiFi.h>

const char ssid[] = "xxx";
const char pass[] = "yyy";

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, pass);

  Serial.print("Connecting to: ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED)  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print(":DD");
}

void loop() {}

// Example: when the wifi is connected, turn ON a led.
