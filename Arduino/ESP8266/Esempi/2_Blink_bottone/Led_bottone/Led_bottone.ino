/*
 * Il programma gestisce l'accesione d'un led
 * tramite la pressione d'un bottone.
 * Il risultato è stampato in seriale.
 */
 
#define led D4
#define button D5 

void setup() {
  pinMode(button,INPUT_PULLUP);
  pinMode(led,OUTPUT);
  Serial.begin(9600);

}

void loop() {
  if(digitalRead(button)==1){
    delay(15);
    Serial.println("Bottone premuto.");
    digitalWrite(led, HIGH);
    delay(500);
  }
  else{
  Serial.println("Bottone NON premuto.");
  digitalWrite(led, LOW);
  delay(500);
  }
}
