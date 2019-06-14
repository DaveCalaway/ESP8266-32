/*
  Version 0.3
  Send the button status to the browser by the ESP8266.
  ESP in access point Mode: https://goo.gl/KLe9pm
  
  Reference page: http://192.168.4.1
*/
#include <ESP8266WiFi.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "123456789";
const char AP_NameChar[] = "ESP8266";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = D4; // internal led INVERTED
const int BUTTON = D5;  
int val = 0;
WiFiServer server(80);
void setupWiFi();

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON, INPUT);
  digitalWrite(LED_PIN, HIGH); // led inverted
  setupWiFi();
  server.begin();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  
  //client.flush();

  if (digitalRead(BUTTON) == HIGH) {
    digitalWrite(LED_PIN, LOW);
    val = 1;
  }
  else {
    digitalWrite(LED_PIN, HIGH);
    val = 0;
  }
  
  // client ready?
  if (!client) {
      return;
    }
  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // Creation of answers
  if (val == 1)  {
    s += "Led: ON ";
  }
  if (val == 0)  {
    s += "Led: OFF ";
  }
  if(val != 0 && val != 1)
    s += "Invalid Request.<br>";

  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);

}

void setupWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}
