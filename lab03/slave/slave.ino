#include <Wire.h>

#define SLAVE_ADDR 8;

const int YELLOW = 4;
const int RED = 3;
const int GREEN = 2;

const int TEMP_THRESHOLD = 27;           // degrees celsius
const unsigned long MIN_PERIOD = 200;    // 0.2 seconds
const unsigned long MAX_PERIOD = 2000;   // 2 seconds

const unsigned long CALIBRATION_DURATION = 5000; // 5 seconds

int angle, lightIntensity;
float temperature;

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);

  Wire.begin(SLAVE_ADDR);

  Serial.begin(9600); // debug purposes
}

void loop() {
}


void handleTemperature() {

  if (temperature > TEMP_THRESHOLD) {
    digitalWrite(YELLOW, HIGH);
  } else if (temperature < TEMP_THRESHOLD) {
    digitalWrite(YELLOW, LOW);
  }

}

void handleLight() {

  unsigned long delayBlink;

  // lightValue defines the intensity of the ligth (https://www.arduino.cc/en/Tutorial/PWM)
  delayBlink = millis();
  analogWrite(RED, 255 - lightIntensity);

}


void handlePot() {

  static unsigned long delayBlink;

  unsigned long period = map(angle, 0, 180, MIN_PERIOD, MAX_PERIOD);
  unsigned long t = millis();
  unsigned long dt = t - delayBlink;

  if ( dt < (period / 2) ) {
    digitalWrite(GREEN, HIGH);
  } else if ( (dt > (period / 2)) && (dt < period) ) {
    digitalWrite(GREEN, LOW);
  } else {
    delayBlink = millis();
  }

}
