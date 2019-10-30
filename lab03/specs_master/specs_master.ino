#include <Wire.h>

#define SLAVE_ADDR 8
#define SIZE 1000

void setup() {  
  Wire.begin();
  
  Serial.begin(9600); // debug purposes
}

void loop() {
  // latency();
  bandwidth();
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
  int start = millis();

  Wire.beginTransmission(SLAVE_ADDR);
  for (int i = 0; i < SIZE; i++) {
    Wire.write('A'); 
  }
  Wire.endTransmission();

  int res;  

  Wire.requestFrom(SLAVE_ADDR, 1);

  while (Wire.available()) {
    res = Wire.read();
  }

  int end = millis();

  if (res) { Serial.println("OK"); }
  else { Serial.println("NOK"); }
  
  Serial.print("time = ");
  Serial.print(end - start);
  Serial.println(" ms");

  Serial.print((float) SIZE / (0.001 * (end - start))); 
  Serial.println(" bytes/s");
}
