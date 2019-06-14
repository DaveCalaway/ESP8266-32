## Quick tutorial for ESP8266 and Telegram bot
This is a short guide, step by step for integrat the ESP8266 with a Telegram bot.  
Demo video: https://youtu.be/6cgZ9mfWIxA

### Arduino IDE &  ESP8266

 1. Install Arduino IDE: https://goo.gl/yPNVju   
 2. Add the ESP8266 Arduino support: https://goo.gl/yAqlU4
 3. Search those library in "Sketch - Include library - manage libraries" : 
	 1.  ArduinoJson
	 2.  UniversalTelegramBot - Features form library: https://goo.gl/FrSUD3

Now we need to jump on the Telegram.

### Set up the telegram bot
First install telegram on your phone, next search the user called "@botfather" .
Start a conversation with him. He show to you a list of command.
Don't worry, follow me.

Start a **new bot** with " /newbot" .
He want some information about your project:

 - The *Name* of your bot will be displayed in contact details and elsewhere.
 - The *Username* is a short name, to be used in mentions and telegram.me links. Your bot's username must end in ‘bot’, e.g. ‘tetris_bot’ or ‘TetrisBot’.

Now the botfather give to you a **token** , this is import for your project! This code connect you and the unique connection to your bot.

Put this token inside the arduino code, in the line 18.

Now you have a new conversation with your bot.

### Arduino code
We are ready to upload the code called **FlashLed** on the ESP8266.

Open FlashLed.ino on the Arduino IDE, select the current ESP8266 version board from the menu " Tool - Board " and upload it.

**What does it do this code?**  
First of all the ESP8266 need to *connect* to your *WiFi* network. Please add the *ssid* and *password* on the sketch.

Using the *token* the bot will connect to your contact on telegram.

In the *Setup* the ESP8266 connects to your Wifi, open the serial port - if you used it - and initialize the led on the board - for LoLin is a buildin led at D4 - .

In the *Loop* it checks every "Bot_lasttime" if one message has arrived.

When the message arrived, it calls the *handleNewMessages* function and extract the *chat_id*, *text* and *from_name* from the message.
Now it jump at *msgInterpretation* the function for understand the message.

**What commands?**  
According with *msgInterpretation* function the bot can understand:

 - /ledon
 - /ledoff
 - /status
