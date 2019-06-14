/*
  Version 0.3
  Send data to the ESP8266 by the browser.
  ESP in access point Mode: https://goo.gl/KLe9pm
 
   Reference page:
    http://192.168.4.1/welcome
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

WiFiServer server(80);
void setupWiFi();

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // led inverted
  setupWiFi();
  server.begin();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  // client ready?
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
  // request type (read/set) and value if set.
  if (req.indexOf("/welcome") != -1)
    val = 0;
  if (req.indexOf("/ON") != -1)
    val = 1;
  if  (req.indexOf("/OFF") != -1)
    val = 2;
  // Otherwise request will be invalid. We'll say as much in HTML

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // Creation of answers
  if (val == 0)  {
    s += "Benvenuto: ";
    s += "<br>"; // Go to the next line.
    s += "Accensione led tramite HTTP <br>";
  }
  if (val == 1)  {
    s += "Led: ON ";
    digitalWrite(LED_PIN, LOW); // led inverted
  }
  if (val == 2)  {
    s += "Led: OFF ";
    digitalWrite(LED_PIN, HIGH); // led inverted
  }
  if(val != 1 && val != 2)
    s += "Invalid Request.<br>";

  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

void setupWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}
