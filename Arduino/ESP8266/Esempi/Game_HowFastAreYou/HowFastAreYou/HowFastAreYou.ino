/*
 * Version : 0.4
 * Reference page:
 *  http://192.168.4.1/welcome
*/
#include <ESP8266WiFi.h>
// uncomment the next line for enable debug
// #define DEBUG
// uncomment the next line for display mac address
// #define MAC

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "123456789";
const char AP_NameChar[] = "ESP8266";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 4; // D2 led pin
const int button = 5;  // D1

/////////////////////
//   Variables     //
/////////////////////
int best = 1000;
unsigned long counter;
unsigned long old = 0;

WiFiServer server(80);
void initHardware();
#ifdef MAC
  void setupWiFi();
#endif
void HowFastAreYou();

void setup(){
  initHardware();
#ifdef MAC
  setupWiFi();
#endif
  server.begin();
}

void loop(){
  // start game?
  if (digitalRead(button) == HIGH) 
    HowFastAreYou();
  
  #ifdef DEBUG
  Serial.println(counter);
  #endif
  
  // Check if a client has connected
  WiFiClient client = server.available();

  // client ready?
  if (!client) {
    return;
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
  // request type (read/set) and value if set.
  if (req.indexOf("/welcome") != -1)
    val = 0;
  if (req.indexOf("/risultati") != -1)
    val = 1;

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // Creation of answers
  if (val == 0)  {
    s += "Benvenuto: ";
    s += "<br>"; // Go to the next line.
    s += "Who fast are you? <br>";
    s += "Premi il bottone per iniziare. <br>";
    s += "Visualizza il tuo risultato su http://192.168.4.1/risultati";
    //s += (val)?"on":"off";
  }
  if (val == 1)  { 
    s += "Risultati: ";
    s += String(counter);
    s += " secondi.";
    if (counter < best) {
        best = counter;
        s += "<br>";
        s +="Best time reactions!";
    }
  }
  if(val != 0 && val != 1)  
    s += "<br> Invalid Request.<br> Try /welcome or /risultati .";
  
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  #ifdef DEBUG
  Serial.println("Client disonnected");
  #endif
}

#ifdef MAC
void setupWiFi(){
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i = 0; i < AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}
#endif

void initHardware(){
  Serial.begin(115200);
  pinMode(button, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}
