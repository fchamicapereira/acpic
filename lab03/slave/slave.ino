#include <Wire.h>

#define SLAVE_ADDR 8

const int YELLOW = 4;
const int RED = 3;
const int GREEN = 2;

const int TEMP_THRESHOLD = 27;           // degrees celsius
const unsigned long MIN_PERIOD = 200;    // 0.2 seconds
const unsigned long MAX_PERIOD = 2000;   // 2 seconds

const unsigned long CALIBRATION_DURATION = 5000; // 5 seconds

int angle, lightIntensity, temperature;
unsigned long period;

int OP_LIGHT = 0;
int OP_TEMP = 1;
int OP_POT = 2;

bool fTaskTemp = false;
bool fTaskLight = false;
bool fTaskPot = false;

void receiveEvent(int howMany) {
  int value;
  
  while (Wire.available() == 0) {}

  int op = Wire.read();
  Serial.print("Operation");
  Serial.println(op);
  
  while (Wire.available()) {
    value = Wire.read();
    Serial.print("received ");
    Serial.println(value);
  }

  if (op == OP_LIGHT) {
    lightIntensity = value;
    fTaskLight = true;
  } else if (op == OP_TEMP) {
    temperature = value;
    fTaskTemp = true;
  } else if (op == OP_POT) {
    angle = value;
    fTaskPot = true;
  } else {
    Serial.print("Unknown operation ");
    Serial.println(op);
  }
}

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);

  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);

  Serial.begin(9600); // debug purposes
}

void loop() {
  if (fTaskPot) {
    fTaskPot = false;
    handlePot();
  }

  blink();
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

  Serial.print("Angle stored = ");
  Serial.println(angle);
  Serial.println(" º");

  period = map(angle, 0, 180, MIN_PERIOD, MAX_PERIOD);
}

void blink() {
  static unsigned long delayBlink;

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
