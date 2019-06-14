void HowFastAreYou() {

  // genero random number
  int ran = random(200, 1000);
  delay(ran);

  // salvo lo stato iniziale
  old = millis();

  digitalWrite(LED_PIN, LOW); // inverted
  int push = 0;
  while ( push == 0 ) {
    yield(); // ESP8266 delay for internal interrupts
    push = digitalRead(button);
  }

  // salvo lo stato finale
  counter = millis();
  // sottraggo per avere tempo di reazione
  counter = counter - old;

  digitalWrite(LED_PIN, HIGH); // inverted
  delay(250);
}

