#define onePin 13 
#define twoPin 12 

// setting PWM properties
const int freq = 5000; // Period: 1/f
const int oneChannel = 0; // Channels 0-16
const int twoChannel = 1;
const int resolution = 10; //Resolution 8, 10, 12, 15


void setup() {
  // setup the PLLs
  ledcSetup(oneChannel, freq, resolution);
  ledcSetup(twoChannel, freq, resolution);

  // attach the channel to the GPIO
  ledcAttachPin(onePin, oneChannel);
  ledcAttachPin(twoPin, twoChannel);

}

void loop() {
  ledcWrite(oneChannel, 512); // duty cycle 50%
  ledcWrite(twoChannel, 256); // duty cycle 25%
}
