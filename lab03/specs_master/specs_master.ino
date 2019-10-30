#include <Wire.h>

#define SLAVE_ADDR 8
#define SIZE 32

void setup() {  
  Wire.begin();
  
  Serial.begin(9600); // debug purposes
}

void loop() {
  latency();
  delay(5000);
  bandwidth();
  delay(5000);
}

void latency() {
  Serial.println("*** Latency ***");
  
  Wire.beginTransmission(SLAVE_ADDR);
  for (int i = 0; i < 32; i++) {
    Wire.write('a'); 
  }
  Wire.endTransmission();

  int start = millis();
  int received;
  
  Wire.requestFrom(SLAVE_ADDR, 1);

  while (Wire.available() == 0) {}
  int end = millis();
  
  while (Wire.available()) {
    received = Wire.read();
  }

  Serial.print("received ");
  Serial.print(received);
  Serial.println(" bytes");
  
  Serial.print("time = ");
  Serial.print(end - start);
  Serial.println(" ms");
}

void bandwidth() {
  Serial.println("*** Bandwidth***");
  
  int received = 0;
  int start = millis();

  Wire.beginTransmission(SLAVE_ADDR);
  for (int i = 0; i < SIZE; i++) {
    Wire.write('A'); 
  }
  Wire.endTransmission();

  Wire.requestFrom(SLAVE_ADDR, 1);

  while (Wire.available() == 0) { delay(10); }

  while (Wire.available()) {
    received = Wire.read();
  }

  int end = millis();

  if (received) { Serial.println("OK"); }
  else { Serial.println("NOK"); }
  
  Serial.print("time     = ");
  Serial.print(end - start);
  Serial.println(" ms");

  Serial.print("received = ");
  Serial.print(received);
  Serial.println(" bytes");

  int speed = 0.01 * received / (0.001 * (end - start));

  Serial.print("speed    = ");
  Serial.print(speed); 
  Serial.println(" KB/s");
}
