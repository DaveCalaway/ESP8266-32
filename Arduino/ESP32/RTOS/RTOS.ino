#include <WiFi.h>
#define ledPin 2  // internal LED

const char* ssid = "xxx";
const char* pass = "yyy";

unsigned int counter = 0;
unsigned long previousMillis = 0;
const long interval = 1000;


void codeForTask1(void *parameter);  // Function Declaration, now i can put the function on the bottom of the code

// --- Cores ---
TaskHandle_t Task1;
SemaphoreHandle_t lock = 0;


//
// --- WIFI CON ---
//
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WiFi.SSID());

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(150);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


//
// --- BOTH CYLINDER ---
//
void bothCylinder() {
  xTaskCreatePinnedToCore(
    codeForTask1, /* Task function. */
    "Task_1",     /* name of task. */
    2000,         /* Stack size of task */
    NULL,         /* parameter of the task */
    1,            /* priority of the task */
    &Task1,       /* Task handle to keep track of created task */
    0);           /* Core */

  //delay(500);  // needed to start-up task1

  // xTaskCreatePinnedToCore(
  //     codeForTask2,
  //     "Task_2",
  //     1000,
  //     NULL,
  //     1,
  //     &Task2,
  //     1);
}


//
// --- SETUP ---
//
void setup() {
  lock = xSemaphoreCreateMutex();
  bothCylinder();

  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
}


//
// --- TASK1 ON CORE0---
//
void codeForTask1(void *parameter) {
  for (;;) {
    Serial.println("          CORE0 RUN!");
    //Serial.printf("         TASK1 on Core %d\n ", xPortGetCoreID());

    if (xSemaphoreTake(lock, 50)) {
      counter++;
      xSemaphoreGive(lock);
    } else
      Serial.println("            Core0 access variables fail.1");

    vTaskDelay(50);
  }
}


//
// --- LOOP ON CORE1---
// -- WiFi --
//
void loop() {
  //Serial.printf("Loop on Core %d\n ", xPortGetCoreID());

  // check the WiFi
  digitalWrite(ledPin, HIGH);
  if (WiFi.status() != WL_CONNECTED) setup_wifi();  // if not connected
  digitalWrite(ledPin, LOW);

  if (millis() - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = millis();
    unsigned int _counter;

    if (xSemaphoreTake(lock, 50)) {
      _counter = counter;
      xSemaphoreGive(lock);
    } else Serial.println("Core1 access variables fail.");

    Serial.println(_counter);
  }
}
