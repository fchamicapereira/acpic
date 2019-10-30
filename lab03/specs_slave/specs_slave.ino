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

void ok() {
  Wire.write(1);
  Serial.println("OK");
}

void receiveEvent(int howMany) {
  Serial.println("Receiving...");
  
  while (1 < Wire.available()) {
    received = Wire.read();
  }
  
  ok();
}

void requestEvent() {
  Wire.write(received);
}
