// Every msg length the bot check if one message has arrived
void check_msg() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    Serial.println("got response");
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}


// New message come to the bot
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  // Extracting info from the message
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    // Is it a DISP function?
    if ( text.startsWith(Disp) == 1 ) {
      clean = 0;
      string = text.substring(6);
      Serial.println("Display: " + string);
    }

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    // Call the function for understand the message
    msgInterpretation(from_name, text, chat_id);
  }
}
