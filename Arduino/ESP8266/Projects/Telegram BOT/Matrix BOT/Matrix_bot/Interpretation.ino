// We have the information from the msg, now we need to understand it
void msgInterpretation(String from_name, String text, String chat_id) {
  //  if (text == "/ledon") {
  //    digitalWrite(ledPin, HIGH);
  //    ledStatus = 1;
  //    bot.sendMessage(chat_id, "Led is ON", "");
  //  }
  //
  //  if (text == "/ledoff") {
  //    ledStatus = 0;
  //    digitalWrite(ledPin, LOW);
  //    bot.sendMessage(chat_id, "Led is OFF", "");
  //  }

  if (text == "/status") {
    bot.sendMessage(chat_id, "System status: ONLINE", "");
  }

  if (text == "/options") {
    //String keyboardJson = "[[\"/ledon\", \"/ledoff\"],[\"/disp payload\"],[\"/status\"]]";
    String keyboardJson = "[[\"/start\"],[\"/disp payload\"],[\"/status\"]]";
    bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
  }


  if (text == "/start") {
    String welcome = "Welcome to Matrix Arduino Telegram Bot, " + from_name + ".\n";
    welcome += "The open-source example: https://goo.gl/TAq3YG \n\n";
    //welcome += "/ledon : to switch the Led ON\n";
    //welcome += "/ledoff : to switch the Led OFF\n";
    welcome += "/disp paylod : display the payload in a matrix\n";
    welcome += "/status : Returns current status\n\n";
    bot.sendMessage(chat_id, welcome, "Markdown");
  }
}

