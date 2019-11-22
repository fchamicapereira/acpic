#include <Wire.h>

#define SLAVE_ADDR 8

const int TEMP_SENSOR = A0;
const int LIGHT_SENSOR = A1;
const int POT_SENSOR = A3;

int tempValue = 0;     // temperature sensor value
int tempErr = 0;

int lightValue = 0;    // light sensor value
int lightMax = 1023;      // maximum light sensor value

int potValue = 0;      // potentiometer value
int potMin = 1023;     // minimum potentiometer value
int potMax = 0;        // maximum potentiometer value

int OP_LIGHT = 0;
int OP_TEMP = 1;
int OP_POT = 2;

const unsigned long CALIBRATION_DURATION = 5000; // 5 seconds
const unsigned long CALIBRATION_ERROR_VARIATION = 3000; // 1 second

void send(int op, int value) {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(op);
  Wire.write(value);
  Wire.endTransmission();
}

void calibrateLight() {
  Serial.println();
  Serial.print("Light calibration started... ");

  unsigned long start = millis();
  
  while ((millis() - start) <= CALIBRATION_DURATION) {
    lightValue = analogRead(LIGHT_SENSOR);
  
    // record the minimum and maximum light sensor value
    if (lightValue < lightMax) { lightMax = lightValue; }
  }

  Serial.println("done");
}

void calibratePot() {
  Serial.print("Pot calibration started... ");

  unsigned long start = millis();
  
  while ((millis() - start) <= CALIBRATION_DURATION) {
    potValue = analogRead(POT_SENSOR);

    // record the minimum and maximum potentiometer value
    if (potValue > potMax) { potMax = potValue; }
    if (potValue < potMin) { potMin = potValue; }
  }

  Serial.println("done");  
}

void calibrateTemp() {

  Serial.print("Temp calibration started... ");
  
  unsigned long start = millis();
  int tempMin = 1024;
  int tempMax = 0;

  while ((millis() - start) <= CALIBRATION_ERROR_VARIATION) {
    tempValue = analogRead(TEMP_SENSOR);

    // record the minimum and maximum potentiometer value
    if (tempValue > tempMax) { tempMax = tempValue; }
    if (tempValue < tempMin) { tempMin = tempValue; }
  }

  tempErr = tempMax - tempMin;
  
  Serial.println("done");
}

void setup() {   
  Wire.begin();
  
  Serial.begin(9600); // debug purposes
    
  /*
   * Calibration techine:
   * The board takes sensor readings for 15 seconds during the startup, and
   * tracks the highest and lowest values it gets. These sensor readings during
   * the first five seconds of the sketch execution define the minimum and maximum
   * of expected values for the readings taken during the loop.
  */

  Serial.println();
  calibrateLight();
  calibratePot();
  calibrateTemp();
  Serial.println();
}

void loop() {
  Serial.println();
  
  readTempSensor();
  readPotSensor();
  readLightSensor();
  
  Serial.println();
}

void readTempSensor() {

  static int prevTemp;

  // https://www.arduino.cc/en/uploads/Main/TemperatureSensor.pdf

  // read the sensors
  tempValue = analogRead(TEMP_SENSOR);

  if (abs(tempValue - prevTemp) <= tempErr) {
    tempValue = prevTemp;
  }

  // getting the voltage from the value read from the sensor
  float voltage = (float) tempValue / 1024.0 * 5.0;
  
  // converting from 10 mv per degree with 500 mV offset
  int temperature = (voltage - 0.5) * 100;

  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" ºC");
  send(OP_TEMP, temperature);

  prevTemp = tempValue;
}

void readLightSensor() {
  
  lightValue = analogRead(LIGHT_SENSOR);
  
  lightValue = map(lightValue, 0, 255, 0, 255);
  int lightIntensity = constrain(lightValue, 0, 255);
  
  Serial.print("Light intensity: "); Serial.println(lightIntensity);
  send(OP_LIGHT, lightIntensity);
}

void readPotSensor(){

  potValue = analogRead(POT_SENSOR);
  int angle = map(potValue, potMin, potMax, 0, 180);

  angle = constrain(angle, 0, 180);
  Serial.print("Angle: "); Serial.print(angle); Serial.println(" º");
  send(OP_POT, angle);
}
