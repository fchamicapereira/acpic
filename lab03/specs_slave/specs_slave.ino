#include <Wire.h>

#define SLAVE_ADDR 8

char received;

void setup() {
  Wire.begin(SLAVE_ADDR);
  
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(9600); // debug purposes
}

void loop() {
}

void receiveEvent(int howMany) {
  while (1 < Wire.available()) {
    received = Wire.read();
  }
  
  int x = Wire.read();
}

void requestEvent() {
  Wire.write(received);
}
