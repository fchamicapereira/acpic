const int YELLOW = 4;
const int RED = 3;
const int GREEN = 2;

const int TEMP_SENSOR = A0;
const int LIGHT_SENSOR = A1;
const int POT_SENSOR = A3;

const int TEMP_THRESHOLD = 26;           // degrees celsius
const unsigned long MIN_PERIOD = 200;    // 0.2 seconds
const unsigned long MAX_PERIOD = 2000;   // 2 seconds

const unsigned long CALIBRATION_DURATION = 5000; // 5 seconds
const unsigned long TASK_SLICE = 200; // 200ms for each task

int tempValue = 0;     // temperature sensor value
int tempMin = 10000;   // minimum temperature sensor value
int tempMax = -10000;  // maximum temperature sensor value

int lightValue = 0;    // light sensor value
int lightMin = 1023;   // minimum light sensor value
int lightMax = 0;      // maximum light sensor value

int potValue = 0;      // potentiometer value
int potMin = 1023;     // minimum potentiometer value
int potMax = 0;        // maximum potentiometer value

unsigned long period;

unsigned long savedT; // start of the time slot of the task

void runTask(void (*task)()) {
  
  // mark the beginning of the task execution
  savedT = millis();

  // get the time to end this task
  unsigned long endT = savedT + TASK_SLICE;

  // execute the task while it's its time to execute
  while (millis() < endT) {
    task();
  }

}

void calibrateTemperature(){
  Serial.println("Temperature calibration started");

  unsigned long start = millis();
  
  while ((millis() - start) <= CALIBRATION_DURATION) {
    tempValue = analogRead(TEMP_SENSOR);
  
    // record the minimum and maximum temperature sensor value
    if (tempValue > tempMax) { tempMax = tempValue; }
    if (tempValue < tempMin) { tempMin = tempValue; }
  }

  Serial.println("Temperature calibration finnished");  
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

  Serial.println("Light calibration finnished");
}

void calibratePot(){
  Serial.println("Pot calibration started");

  unsigned long start = millis();
  
  while ((millis() - start) <= CALIBRATION_DURATION) {
    potValue = analogRead(POT_SENSOR);
  
    // record the minimum and maximum light sensor value
    if (lightValue > lightMax) { lightMax = lightValue; }
    if (lightValue < lightMin) { lightMin = lightValue; }

    // record the minimum and maximum potentiometer value
    if (potValue > potMax) { potMax = potValue; }
    if (potValue < potMin) { potMin = potValue; }
  }

  Serial.println("Pot calibration finnished");  
}

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  
  Serial.begin(9600); // debug purposes
    
  /*
   * Calibration techine:
   * The board takes sensor readings for five seconds during the startup, and
   * tracks the highest and lowest values it gets. These sensor readings during
   * the first five seconds of the sketch execution define the minimum and maximum
   * of expected values for the readings taken during the loop.
  */
  
  calibrateTemperature();
  calibrateLight();
  calibratePot();
}

void loop() {
  runTask(controlTemperature); 
  runTask(controlRotationAngle);
  runTask(controlLightItensity);
}


void controlTemperature(){
  float temp;

  // read the sensors
  tempValue = analogRead(TEMP_SENSOR);
 
  // in case the sensor value is outside the range seen during calibration
  tempValue = constrain(tempValue, tempMin, tempMax);  
  temp = T(tempValue);
  
  Serial.print("Temperature: "); Serial.print(temp); Serial.println(" ºC");
    
  if (temp > TEMP_THRESHOLD) {
    digitalWrite(YELLOW, HIGH);
  } else {
    digitalWrite(YELLOW, LOW);
  }
  
}


void controlLightItensity(){

  static unsigned long delayBlink;
  
  lightValue = analogRead(LIGHT_SENSOR);
  lightValue = map(lightValue, lightMin, lightMax, 0, 255);
  lightValue = constrain(lightValue, 0, 255);
  
  Serial.print("Light intensity: "); Serial.print(lightValue); Serial.println(" light intensity");

  //lightValue defines the intensity of the ligth (https://www.arduino.cc/en/Tutorial/PWM) 
  if((millis() - delayBlink) == 200 )
  {
    
    delayBlink = millis();
    analogWrite(RED, lightValue);
  }
 
}


void controlRotationAngle(){

  static unsigned long delayBlink;  
  float deg;

  potValue = analogRead(POT_SENSOR);
  deg = map(potValue, potMin, potMax, 0, 180);

  deg = constrain(deg, 0, 180);
  Serial.print("Degrees: "); Serial.print(deg); Serial.println(" º");

  period = P(deg);
  
   if ( (millis() - delayBlink) < period ) {
      digitalWrite(GREEN, HIGH);
  } else {
      digitalWrite(GREEN, LOW);
      delayBlink = millis();
  }

}

float T(int value) {
  
  // getting the voltage from the value read from the sensor
  float voltage = value / 1024.0 * 5.0;
  
  // converting from 10 mv per degree with 500 mV offset
  float temperature = (voltage - 0.5) * 100;

  return temperature; 
}

float P(int value) {
   return 0.01 * value - 0.2; 
}
