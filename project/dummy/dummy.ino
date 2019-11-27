#include <Wire.h>

void recv(int howMany) {
  Serial.println("RECV");
  
  while (Wire.available() > 0) {
    Serial.println("AVAILABLE");
    char c = Wire.read();
    Serial.println("READ ");
    Serial.println(c);
    Serial.print(c);
  }
  Serial.println();
}

void setup() {
  Wire.begin();
  Wire.onReceive(recv);
  Serial.begin(9600);  // start Serial for output
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
}
