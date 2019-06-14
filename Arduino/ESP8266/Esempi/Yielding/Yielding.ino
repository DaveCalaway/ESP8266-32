/*
 * Esempio sull'uso della funzione Yielding
 */
#define button 5 // pin D1

void setup() {
  pinMode(button,INPUT);
}

void loop() {
  while (digitalRead(12) == HIGH){ 
    yield(); 
  }
Serial.println("Button is pressed!"); 
}

/*
 * commentare l'istruzione yield() , cosa succede?
 */
