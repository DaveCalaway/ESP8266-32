/*******************************************************************
    Exercises to control a Led with TelegramBot.

    DaveCalaway
 *******************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Initialize Wifi connection to the router
char ssid[] = "XXXX";     // your network SSID (name)
char password[] = "XXXX"; // your network key

// Initialize Telegram BOT
#define BOTtoken "XXXX"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;

const int ledPin = D4; // ESP8266 LoLin internal led
int ledStatus = 1;  // ESP8266 LoLin have the reverse polarity led


// New message come to the bot
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  // Extracting info from the message
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
    // Call the function for understand the message
    msgInterpretation(from_name, text, chat_id);
  }
}

// We have the information from the msg, now we need to understand it
void msgInterpretation(String from_name, String text, String chat_id) {


  if (text == "/status") {
    if (ledStatus) {
      bot.sendMessage(chat_id, "Led is ON", "");
    } else {
      bot.sendMessage(chat_id, "Led is OFF", "");
    }
  }

  if (text == "/start") {
    String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
    welcome += "This is Flash Led Bot example.\n\n";
    welcome += "/ledon : to switch the Led ON\n";
    welcome += "/ledoff : to switch the Led OFF\n";
    welcome += "/status : Returns current status of LED\n";
    bot.sendMessage(chat_id, welcome, "Markdown");
  }
}


void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH); // initialize pin as off
}

void loop() {
  // Every "Bot_lasttime" the bot check if one message has arrived
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
