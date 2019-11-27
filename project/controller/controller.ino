#include <Wire.h>

#define N_TRAFFIC_LIGHTS 2

#define SN 0
#define WE 1

#define GREEN 0
#define YELLOW 1
#define RED 2

#define TRANSITION_PERCENTAGE 0.1 // 10% of duty cycle

#define INITIAL_TASK_DURATION 6000 // 6 seconds
#define INITIAL_TASK_PERIOD 2000 // 2 seconds

#define STANDBY_TASK_PERIOD 2000 // 2 seconds

#define NORMAL0_TASK_PERIOD 20000 // 20 seconds
#define NORMAL0_TASK_DUTY_CYCLE 0.5 // 50% duty cycle

enum state {
  INITIAL, STANDBY,
  NORMAL0, NORMAL1, NORMAL2
};

typedef struct {
  byte n;
  byte s;
  byte e;
  byte w;
} cars_counter;

typedef struct {
  byte dst;
  byte src;
  byte event;
  cars_counter cars;
  long int t;
} frame;

// south-north and west-east traffic lights
// green, yellow and red
const int TL[N_TRAFFIC_LIGHTS][3] = { { 2, 3, 4 }, { 5, 6, 7 } };
const int CHECKER[N_TRAFFIC_LIGHTS][3] = { { -1, -1, 8 }, { -1, -1, 9 } };

const int LD_SN = 12;
const int LD_WE = 13;

state tlState = INITIAL;

bool checkMalfunction(int tl, int color, int expected) {
  if (CHECKER[tl][color] == -1) return true;

  int current = digitalRead(CHECKER[tl][color]);

  Serial.print("Check tl ");
  Serial.print(tl);
  Serial.print(" color ");
  Serial.print(color);
  Serial.print(" current ");
  Serial.print(current);
  Serial.print(" expected ");
  Serial.print(expected);
  Serial.print(" equal ");
  Serial.print(current == expected);
  Serial.println();
  
  if (current != expected) tlState = STANDBY;

  return current == expected;
}

// traffic lights' task
void tlTask() {  
  switch (tlState) {
    case INITIAL:
      Serial.println("Initial");
      initialTask(SN);
      initialTask(WE);
      break;

    case STANDBY:
      Serial.println("Stand by");
      standByTask(SN);
      standByTask(WE);    
      break;
      
    case NORMAL0:
      Serial.println("Normal0");
      normal0Task(SN);
      normal0Task(WE);
      break;
      
    case NORMAL1:
      break;
      
    case NORMAL2:
      break;
  }
}

void initialTask(int tl) {
  static unsigned long delay[N_TRAFFIC_LIGHTS] = { millis(), millis() };
  
  unsigned long dt = millis() - delay[tl];
  unsigned long globalDt = millis();
  
  int light = TL[tl][YELLOW];

  if (globalDt > INITIAL_TASK_DURATION) {
    tlState = NORMAL0;
    return;
  }

  if (dt < INITIAL_TASK_PERIOD / 2) {
    digitalWrite(light, HIGH);
  } else if (dt > INITIAL_TASK_PERIOD) {
    delay[tl] = millis();
  } else {
    digitalWrite(light, LOW);
  }
}

void standByTask(int tl) {
  static unsigned long delay[N_TRAFFIC_LIGHTS] = { millis(), millis() };
  unsigned long dt = millis() - delay[tl];
  int light = TL[tl][YELLOW];

  digitalWrite(TL[tl][GREEN], LOW);
  digitalWrite(TL[tl][RED], LOW);

  if (dt < STANDBY_TASK_PERIOD / 2) digitalWrite(light, HIGH);
  else if (dt > INITIAL_TASK_PERIOD) delay[tl] = millis();
  else digitalWrite(light, LOW);
}

void normal0Task(int tl) {
  static unsigned long delay[N_TRAFFIC_LIGHTS] = { millis(), millis() };
  unsigned long dt = millis() - delay[tl];
  
  if (dt < NORMAL0_TASK_PERIOD * NORMAL0_TASK_DUTY_CYCLE * (1 - TRANSITION_PERCENTAGE)) {
    Serial.print(dt); Serial.println(" One");
    digitalWrite(TL[tl][YELLOW], LOW);
    Serial.print("tl "); Serial.print(tl); Serial.print(" color "); Serial.println(GREEN);
    digitalWrite(TL[tl][GREEN], HIGH);
    if (!checkMalfunction(tl, RED, LOW)) return;
    return;
  }
  
  if (dt < NORMAL0_TASK_PERIOD * NORMAL0_TASK_DUTY_CYCLE) {
    Serial.print(dt); Serial.println(" Two");
    digitalWrite(TL[tl][GREEN], LOW);
    digitalWrite(TL[tl][YELLOW], HIGH);
    if (!checkMalfunction(tl, RED, LOW)) return;
    return;
  }

  if (dt < NORMAL0_TASK_PERIOD -
    (NORMAL0_TASK_PERIOD * NORMAL0_TASK_DUTY_CYCLE * TRANSITION_PERCENTAGE)) {
    Serial.print(dt); Serial.println(" Three");
    digitalWrite(TL[tl][YELLOW], LOW);
    digitalWrite(TL[tl][RED], HIGH);
    if (!checkMalfunction(tl, RED, HIGH)) return;
    return;
  }

  if (dt < NORMAL0_TASK_PERIOD) {
    Serial.print(dt); Serial.println(" Four");
    digitalWrite(TL[tl][RED], LOW);
    digitalWrite(TL[tl][YELLOW], HIGH);
    if (!checkMalfunction(tl, RED, LOW)) return;
    return;
  }

  delay[tl] = millis();
}

void receiveFrame(int howMany) {
  frame f;
  
  while (Wire.available()) {
    Wire.readBytes((byte*)&f, sizeof(frame));
  }
}

void setup() {
  Serial.begin(9600); // debug purposes
  
  pinMode(TL[SN][GREEN], OUTPUT);
  pinMode(TL[SN][YELLOW], OUTPUT);
  pinMode(TL[SN][RED], OUTPUT);

  pinMode(TL[WE][GREEN], OUTPUT);
  pinMode(TL[WE][YELLOW], OUTPUT);
  pinMode(TL[WE][RED], OUTPUT);

  pinMode(CHECKER[SN][RED], INPUT);
  pinMode(CHECKER[WE][RED], INPUT);

  pinMode(LD_SN, INPUT);
  pinMode(LD_WE, INPUT);

  // join the I2C bus
  // Wire.begin();
  // Wire.onReceive(receiveFrame);
}

void loop() {
  tlTask();
}
