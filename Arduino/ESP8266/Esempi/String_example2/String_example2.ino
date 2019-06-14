/*
 * Esempio di StringObject
 * Popolo stringa, passo passo ed in fine la stampo.
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

void loop(){
// creo stringa vuota
  String stringOne;
  String nome = "Dave";
  
  stringOne += "Salve sono " + nome;

  int anni = 43;

  stringOne += " ed ho ";
  stringOne += anni;
  
  Serial.println(stringOne);
  delay(5000);
}

/*
 * Perchè non ho potuto stampare gli anni insime alla stringa "ed ho"?
 */
