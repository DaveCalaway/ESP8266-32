/*******************************************************************
    ---- Matrix Bot ----
    This code receive a massage from a Telegram Bot and respond of a specific requests.
    One of this command can display a msg on the Matrix display.
    
    When the device is new for the network, the BOT becomes a wifi station.
    You can connect to the wifi station and enter the credentials of your network.

    Library needed: 
      Telegram bot:
        - UniversalTelegramBot: https://goo.gl/FrSUD3
        - ArduinoJson: https://goo.gl/2bQ04G
      Dot matrix:
        - MAX7219LedMatrix: https://goo.gl/Z8tLTz
      WiFi manager:
        - WiFiManager: https://goo.gl/8qTZj6
      
    Quick tutorial, with easy example, for ESP8266 and Telegram bot: https://goo.gl/SjpKjz

    Davide Gariselli (https://github.com/DaveCalaway) .
*******************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ----- Wifi Manager -----
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// ------ Matrix Display ------
#include <cp437font.h>
#include <LedMatrix.h>
#define NUMBER_OF_DEVICES 1
#define CS_PIN 2 // D4
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN); // D5 clk - D7 din
// ----------------------------

// Initialize Telegram BOT
#define BOTtoken "zzzz"  // your Bot Token (Got from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int text_speed = 60;
bool Start = false;

#define button D1
String string = "<3";
const String Disp = "/disp";
bool scroll = 0;
bool clean = 0;


void setup() {
  Serial.begin(115200);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //reset saved settings
  wifiManager.resetSettings();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180); // 180 seconds = 3 minutes
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("AutoConnect Bot")) {
    Serial.println("failed to connect and hit timeout");
    Serial.println("Deep Sleep");
    ESP.deepSleep(10000000); // 1 second = 1e6
    Serial.println("Start");
  } 

  pinMode(button, INPUT); // initialize digital ledPin as an output.
  delay(10);

  // Matrix init
  ledMatrix.init();
  ledMatrix.setIntensity(3); // range is 0-15
}

void loop() {

  // Display the msg.
  // Check if a new msg have arrived after all old msg is displayed.
  if (scroll == 1 && clean == 0)  {
    check_msg();
  }

  // Button is pressed, check msg after 1 minute
  if (clean == 1) {
    delay(1000);
    check_msg();
  }

  // If button pressed, clear screen
  if (digitalRead(button) == 1) {
    Serial.println("button");
    delay(25);
    clean = 1;
    string = " ";
    while (scroll == 0)    matrix(string); // clean the display
  }

  // print on screen
  matrix(string);
}

