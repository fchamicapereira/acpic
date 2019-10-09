const int YELLOW = 4;
const int RED = 3;
const int GREEN = 2;

const int TEMP_SENSOR = A0;
const int LIGHT_SENSOR = A1;
const int POT_SENSOR = A3;

int tempValue = 0;     // temperature sensor value
int tempMin = 1023;    // minimum temperature sensor value
int tempMax = 0;       // maximum temperature sensor value

int lightValue = 0;    // light sensor value
int lightMin = 1023;   // minimum light sensor value
int lightMax = 0;      // maximum light sensor value

int potValue = 0;      // potentiometer value
int potMin = 1023;     // minimum potentiometer value
int potMax = 0;        // maximum potentiometer value

const int TEMP_THRESHOLD = 26;           // degrees celsius
const unsigned long MIN_PERIOD = 200;    // 0.2 seconds
const unsigned long MAX_PERIOD = 2000;   // 2 seconds

unsigned long period;

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

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  
  Serial.begin(9600); // debug purposes
  
  Serial.println("Calibration started");
  
  /*
   * Calibration techine:
   * The board takes sensor readings for five seconds during the startup, and
   * tracks the highest and lowest values it gets. These sensor readings during
   * the first five seconds of the sketch execution define the minimum and maximum
   * of expected values for the readings taken during the loop.
  */
  
  // calibrate during the first five seconds
  while (millis() < 5000) {
    tempValue = analogRead(TEMP_SENSOR);
    lightValue = analogRead(LIGHT_SENSOR);
    potValue = analogRead(POT_SENSOR);

    // record the minimum and maximum temperature sensor value
    if (tempValue > tempMax) { tempMax = tempValue; }
    if (tempValue < tempMin) { tempMin = tempValue; }
    
    // record the minimum and maximum light sensor value
    if (lightValue > lightMax) { lightMax = lightValue; }
    if (lightValue < lightMin) { lightMin = lightValue; }
    
    // record the minimum and maximum potentiometer value
    if (potValue > potMax) { potMax = potValue; }
    if (potValue < potMin) { potMin = potValue; }
  }
  
  Serial.println("Calibration done");
}

void loop() {
  float temp;
  float deg;
  
  // read the sensors
  tempValue = analogRead(TEMP_SENSOR);
  lightValue = analogRead(LIGHT_SENSOR);
  potValue = analogRead(POT_SENSOR);

  // apply the calibration to the sensors reading
  tempValue = map(tempValue, tempMin, tempMax, 0, 1024);
  lightValue = map(lightValue, lightMin, lightMax, 0, 1024);
  deg = map(potValue, potMin, potMax, 0, 180);

  // in case the sensor value is outside the range seen during calibration
  tempValue = constrain(tempValue, 0, 1024);
  lightValue = constrain(lightValue, 0, 1024);
  deg = constrain(deg, 0, 180);
  
  temp = T(tempValue);
  
  Serial.print("\nTemperature:    "); Serial.print(temp); Serial.println(" ºC");
  Serial.print("Light intensity: "); Serial.print(lightValue); Serial.println(" light intensity");
  Serial.print("Degrees:         "); Serial.print(deg); Serial.println(" º");
  
  if (temp > TEMP_THRESHOLD) {
    digitalWrite(YELLOW, HIGH);
  } else {
    digitalWrite(YELLOW, LOW);
  }
  
  period = P(deg);
  
  digitalWrite(GREEN, HIGH);
  delay(100);
  digitalWrite(GREEN, LOW);

  delay(period);  
}
