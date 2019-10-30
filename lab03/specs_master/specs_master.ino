#include <Wire.h>

#define SLAVE_ADDR 8
#define BUFFER_SIZE 32
#define SIZE BUFFER_SIZE * 100

void setup() {  
  Wire.begin();
  
  Serial.begin(9600); // debug purposes
}

void loop() {
  // latency();
  bandwidth();
  delay(5000);
}

void latency() {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write('a');
  Wire.endTransmission();

  int start = millis();
  char c;
  
  Wire.requestFrom(SLAVE_ADDR, 1);

  while (Wire.available()) {
    c = Wire.read();
  }

  int end = millis();

  Serial.print("received ");
  Serial.println(c);
  
  Serial.print("time = ");
  Serial.print(end - start);
  Serial.println(" ms");
}

void bandwidth() {
  int received = 0;
  int start = millis();

  Wire.beginTransmission(SLAVE_ADDR);
  for (int i = 0; i < SIZE; i += BUFFER_SIZE) {
    Wire.write('A' * BUFFER_SIZE); 
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

  Serial.print(speed); 
  Serial.println(" KB/s");
}
