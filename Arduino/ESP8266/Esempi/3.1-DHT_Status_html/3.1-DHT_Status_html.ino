/*
  DHT_Status_HTML
  Send data to the ESP8266 by the browser.
  ESP in access point Mode: https://goo.gl/KLe9pm

   Reference page:
    http://192.168.4.1/welcome
*/
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "123456789";
const char AP_NameChar[] = "ESP8266";

/////////////////////
// Pin Definitions //
/////////////////////
#define DHTPIN            D5         // Pin which is connected to the DHT sensor.

// Uncomment the type of sensor in use:
#define DHTTYPE           DHT11     // DHT 11 
//#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

WiFiServer server(80);
void setupWiFi();

void setup() {
  setupWiFi();
  server.begin();

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

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
  if (req.indexOf("/HUM") != -1)
    val = 1;
  if  (req.indexOf("/TEMP") != -1)
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
    s += "Digita /TEMP per la temperatura oppure /HUM per l umidità <br>";
  }

  if (val == 1)  {
    sensors_event_t event;

    dht.humidity().getEvent(&event);

    if (isnan(event.relative_humidity)) {
      s += "Error reading humidity! ";
    }
    else {
      s += "Hum: ";
      s += event.relative_humidity;
      s += " %";
    }
  }

  if (val == 2)  {
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      s += "Error reading temperature!";
    }
    else {
      s += "Temperature: ";
      s += event.temperature;
      s += " *C";
    }
  }
  if (val != 1 && val != 2)
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
