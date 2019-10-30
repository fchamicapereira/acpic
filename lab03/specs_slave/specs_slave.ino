#include <Wire.h>

#define SLAVE_ADDR 8

char received;
int size;

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
  Serial.print("OK (");
  Serial.print(size);
  Serial.println(" bytes received)");
}

void receiveEvent(int howMany) {
  Serial.println("Receiving...");

  size = 0;
  while (1 < Wire.available()) {
    received = Wire.read();
    size++;
  }
  
  ok();
}

void requestEvent() {
  Wire.write(received);
}
