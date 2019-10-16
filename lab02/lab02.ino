const int YELLOW = 4;
const int RED = 3;
const int GREEN = 2;

const int TEMP_SENSOR = A0;
const int LIGHT_SENSOR = A1;
const int POT_SENSOR = A3;

const int TEMP_THRESHOLD = 27;           // degrees celsius
const unsigned long MIN_PERIOD = 200;    // 0.2 seconds
const unsigned long MAX_PERIOD = 2000;   // 2 seconds

const unsigned long CALIBRATION_DURATION = 5000; // 5 seconds

int tempValue = 0;     // temperature sensor value
int tempMin = 10000;   // minimum temperature sensor value
int tempMax = -10000;  // maximum temperature sensor value

int lightValue = 0;    // light sensor value
int lightMin = 1023;   // minimum light sensor value
int lightMax = 0;      // maximum light sensor value

int potValue = 0;      // potentiometer value
int potMin = 1023;     // minimum potentiometer value
int potMax = 0;        // maximum potentiometer value

int angle, lightIntensity;
float temperature;

void calibrateTemperature(){
  Serial.println("Temperature calibration started");

  unsigned long start = millis();
  
  while ((millis() - start) <= CALIBRATION_DURATION) {
    tempValue = analogRead(TEMP_SENSOR);
  
    // record the minimum and maximum temperature sensor value
    if (tempValue > tempMax) { tempMax = tempValue; }
    if (tempValue < tempMin) { tempMin = tempValue; }
  }

  Serial.println("Temperature calibration finished");  
}

void calibrateLight(){
  Serial.println("Light calibration started");

  unsigned long start = millis();
  
  while ((millis() - start) <= CALIBRATION_DURATION) {
    lightValue = analogRead(LIGHT_SENSOR);
  
    // record the minimum and maximum light sensor value
    if (lightValue > lightMax) { lightMax = lightValue; }
    if (lightValue < lightMin) { lightMin = lightValue; }
  }

  Serial.println("Light calibration finished");
}

void calibratePot(){
  Serial.println("Pot calibration started");

  unsigned long start = millis();
  
  while ((millis() - start) <= CALIBRATION_DURATION) {
    potValue = analogRead(POT_SENSOR);

    // record the minimum and maximum potentiometer value
    if (potValue > potMax) { potMax = potValue; }
    if (potValue < potMin) { potMin = potValue; }
  }

  Serial.println("Pot calibration finished");  
}

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  
  Serial.begin(9600); // debug purposes
    
  /*
   * Calibration techine:
   * The board takes sensor readings for 15 seconds during the startup, and
   * tracks the highest and lowest values it gets. These sensor readings during
   * the first five seconds of the sketch execution define the minimum and maximum
   * of expected values for the readings taken during the loop.
  */
  
  calibrateTemperature();
  calibrateLight();
  calibratePot();
}

void loop() {
  readTempSensor();
  readPotSensor();
  readLightSensor();

  handleTemperature();
  handleLight();
  handlePot();
}

void readTempSensor() {

  // read the sensors
  tempValue = analogRead(TEMP_SENSOR);
 
  // in case the sensor value is outside the range seen during calibration
  // tempValue = constrain(tempValue, tempMin, tempMax);  

  // getting the voltage from the value read from the sensor
  float voltage = tempValue / 1024.0 * 5.0;
  
  // converting from 10 mv per degree with 500 mV offset
  temperature = (voltage - 0.5) * 100;

  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" ºC");

}


void handleTemperature() {
   
  if (temperature > TEMP_THRESHOLD) {
    digitalWrite(YELLOW, HIGH);
  } else if (temperature < TEMP_THRESHOLD) {
    digitalWrite(YELLOW, LOW);
  }
  
}

void readLightSensor() {
  
  lightValue = analogRead(LIGHT_SENSOR);
  lightValue = map(lightValue, lightMin, lightMax, 0, 255);
  lightIntensity = constrain(lightValue, 0, 255);
  
  Serial.print("Light intensity: "); Serial.print(lightIntensity); Serial.println(" light intensity"); 
  
}

void handleLight() {

  static unsigned long delayBlink;
  
  // lightValue defines the intensity of the ligth (https://www.arduino.cc/en/Tutorial/PWM) 
  if( (millis() - delayBlink) > 200 ) {
    
    delayBlink = millis();
    analogWrite(RED, lightIntensity);
  }

}

void readPotSensor(){

  potValue = analogRead(POT_SENSOR);
  angle = map(potValue, potMin, potMax, 0, 180);

  angle = constrain(angle, 0, 180);
  Serial.print("Angle: "); Serial.print(angle); Serial.println(" º");

}


void handlePot() {

  static unsigned long delayBlink;  
  
  unsigned long period = map(angle, 0, 180, MIN_PERIOD, MAX_PERIOD);
  unsigned long t = millis();
  unsigned long dt = t - delayBlink;
  
   if ( dt < (period / 2) ) {
      digitalWrite(GREEN, HIGH);
  } else if ( (dt > (period / 2)) && (dt < period) ){
      digitalWrite(GREEN, LOW);
  } else {
      delayBlink = millis();
  }

}
