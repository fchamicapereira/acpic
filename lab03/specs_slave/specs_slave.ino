#include <Wire.h>

#define SLAVE_ADDR 8

enum Operation { latency, bandwidth };

int size;

void setup() {
  Wire.begin(SLAVE_ADDR);
  
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(9600); // debug purposes
}

void loop() {
}

void receiveEvent(int howMany) {
  Serial.print("Receiving ");
  Serial.println(howMany);

  size = 0;
  while (Wire.available()) {
    size++;
    Wire.read();
  }
}

void requestEvent() {
  Wire.write(size);
  
  Serial.print("OK (");
  Serial.print(size);
  Serial.println(" bytes received)");
}
