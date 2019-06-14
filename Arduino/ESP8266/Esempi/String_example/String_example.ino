/*
 * Esempio di StringObject
 * Estraggo nome da stringa
 * Riferimento Arduino: https://goo.gl/sBoQGH
 * Esempi: http://goo.gl/PO0BPp
 */

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  // creo stringa e la riempio
  String stringOne = "GET /:gianfrancioschio HTTP/1.1";
  Serial.println("Stringa originale: " + stringOne);
  
  // salvo la posizione di ':'
  int firstClosingBracket = stringOne.indexOf(':');
  /*
  Serial.print(" : e' in posizione " + firstClosingBracket);
  non va bene per la SERIALE, è una striga + int
  strighe con strighe e int con int
  */
  Serial.print(": e' in posizione ");
  Serial.println(firstClosingBracket);
  // tolgo prima parte
  String string2 = stringOne.substring(firstClosingBracket+1);
  Serial.print("Nuova stringa: ");
  Serial.println(string2);
  Serial.println("");

  Serial.println("--> Cancello finale");
  firstClosingBracket = string2.indexOf('H');
  Serial.print("H e' in posizione ");
  Serial.println(firstClosingBracket);
 
  stringOne = string2.substring(0, firstClosingBracket);
  Serial.print("Nuova nome pulita: " + stringOne);
  delay(5000);
 
}

