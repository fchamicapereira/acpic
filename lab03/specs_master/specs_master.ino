#include <Wire.h>

#define SLAVE_ADDR 8

void setup() {  
  Wire.begin();
  
  Serial.begin(9600); // debug purposes
}

void loop() {
  latency();
}

void latency() {
  // Wire.requestFrom(SLAVE_ADDR, 1);
  // while (Wire.available()) { Wire.read(); }
  
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write('a');
  Wire.endTransmission();

  int start = millis();
  
  Wire.requestFrom(SLAVE_ADDR, 1);

  while (Wire.available()) {
    char c = Wire.read();
  }

  int end = millis();

  Serial.print("received ");
  Serial.println(c);
  
  Serial.print("time = ");
  Serial.print(end - start);
  Serial.println(" ms");
}
