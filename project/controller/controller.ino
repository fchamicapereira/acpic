#include <Wire.h>

#define N_TRAFFIC_LIGHTS 2

#define SN 0
#define WE 1

#define GREEN 0
#define YELLOW 1
#define RED 2

#define TRANSITION_TIME 1000 // 1 second

#define INITIAL_TASK_DURATION 6000 // 6 seconds
#define INITIAL_TASK_PERIOD 2000 // 2 seconds

#define STANDBY_TASK_PERIOD 2000 // 2 seconds

#define NORMAL0_TASK_PERIOD 20000 // 20 seconds
#define NORMAL0_TASK_DUTY_CYCLE 0.5 // 50% duty cycle

#define NORMAL1_TASK_PERIOD 20000 // 20 seconds
#define NORMAL1_TASK_DUTY_CYCLE_MIN 0.25 // 25%
#define NORMAL1_TASK_DUTY_CYCLE_MAX 0.75 // 75%

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
const int SENSORS[N_TRAFFIC_LIGHTS] = { 12, 13 };

int CARS[N_TRAFFIC_LIGHTS] = { 0, 0 };

state tlState = INITIAL;

bool checkMalfunction(int tl, int color, int expected) {
  if (CHECKER[tl][color] == -1) return true;

  int current = digitalRead(CHECKER[tl][color]);  
  if (current != expected) tlState = STANDBY;
  return current == expected;
}

// traffic lights' task
void tlTask() {  
  switch (tlState) {
    case INITIAL:
      initialTask(SN);
      initialTask(WE);
      break;

    case STANDBY:
      standByTask(SN);
      standByTask(WE);    
      break;
      
    case NORMAL0:
      normal0Task();
      break;
      
    case NORMAL1:
      normal1Task();
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

void normal0Task() {
  static unsigned long delay = millis();
  unsigned long dt = millis() - delay;
  
  if (dt < NORMAL0_TASK_PERIOD * NORMAL0_TASK_DUTY_CYCLE - TRANSITION_TIME) {
    digitalWrite(TL[SN][YELLOW], LOW);
    digitalWrite(TL[WE][YELLOW], LOW);
    
    digitalWrite(TL[SN][GREEN], HIGH);
    digitalWrite(TL[WE][RED], HIGH);
    
    checkMalfunction(WE, RED, HIGH);
    return;
  }
  
  if (dt < NORMAL0_TASK_PERIOD * NORMAL0_TASK_DUTY_CYCLE) {
    digitalWrite(TL[SN][GREEN], LOW);
    digitalWrite(TL[WE][RED], LOW);
    
    digitalWrite(TL[SN][YELLOW], HIGH);
    digitalWrite(TL[WE][YELLOW], HIGH);

    return;
  }

  if (dt < NORMAL0_TASK_PERIOD - TRANSITION_TIME) {
    digitalWrite(TL[SN][YELLOW], LOW);
    digitalWrite(TL[WE][YELLOW], LOW);

    digitalWrite(TL[SN][RED], HIGH);
    digitalWrite(TL[WE][GREEN], HIGH);
    
    checkMalfunction(SN, RED, HIGH); 
    return;
  }

  if (dt < NORMAL0_TASK_PERIOD) {
    digitalWrite(TL[SN][RED], LOW);
    digitalWrite(TL[WE][GREEN], LOW);
    
    digitalWrite(TL[SN][YELLOW], HIGH);
    digitalWrite(TL[WE][YELLOW], HIGH);
    
    return;
  }

  delay = millis();
}

void normal1Task() {
  static unsigned long delay = millis();
  unsigned long dt = millis() - delay;
  static float duty_cycle = 0.5;
  
  if (dt < NORMAL1_TASK_PERIOD * duty_cycle - TRANSITION_TIME) {
    digitalWrite(TL[SN][YELLOW], LOW);
    digitalWrite(TL[WE][YELLOW], LOW);
    
    digitalWrite(TL[SN][GREEN], HIGH);
    digitalWrite(TL[WE][RED], HIGH);
    
    checkMalfunction(WE, RED, HIGH);
    return;
  }
  
  if (dt < NORMAL1_TASK_PERIOD * duty_cycle) {
    digitalWrite(TL[SN][GREEN], LOW);
    digitalWrite(TL[WE][RED], LOW);
    
    digitalWrite(TL[SN][YELLOW], HIGH);
    digitalWrite(TL[WE][YELLOW], HIGH);

    return;
  }

  if (dt < NORMAL1_TASK_PERIOD - TRANSITION_TIME) {
    digitalWrite(TL[SN][YELLOW], LOW);
    digitalWrite(TL[WE][YELLOW], LOW);

    digitalWrite(TL[SN][RED], HIGH);
    digitalWrite(TL[WE][GREEN], HIGH);
    
    checkMalfunction(SN, RED, HIGH); 
    return;
  }

  if (dt < NORMAL1_TASK_PERIOD) {
    digitalWrite(TL[SN][RED], LOW);
    digitalWrite(TL[WE][GREEN], LOW);
    
    digitalWrite(TL[SN][YELLOW], HIGH);
    digitalWrite(TL[WE][YELLOW], HIGH);
    
    return;
  }
  
  delay = millis(); // end of cycle

  // recalculate duty cycle
  if (CARS[SN] + CARS[WE] > 0)
    duty_cycle = constrain(1.0 * CARS[SN] / (CARS[SN] + CARS[WE]),
      NORMAL1_TASK_DUTY_CYCLE_MIN, NORMAL1_TASK_DUTY_CYCLE_MAX);
  else duty_cycle = 0.5;

  Serial.print("Duty Cycle ");
  Serial.print(duty_cycle);
}

void receiveFrame(int howMany) {
  frame f;
  
  while (Wire.available()) {
    Wire.readBytes((byte*)&f, sizeof(frame));
  }
}

void readSensor(int pos) {
  static bool lastValue[N_TRAFFIC_LIGHTS] = { 0, 0 };

  int value = digitalRead(SENSORS[pos]);

  // no changes
  if (lastValue[pos] == value) return;

  // 0 -> 1 (button pushed)
  if (value == 1) {
    CARS[pos]++; // one is passing
    Serial.print("PUSHED "); Serial.print(pos); Serial.print(" "); Serial.println(CARS[pos]);
  }

  lastValue[pos] = value;
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

  pinMode(SENSORS[SN], INPUT);
  pinMode(SENSORS[WE], INPUT);

  // join the I2C bus
  // Wire.begin();
  // Wire.onReceive(receiveFrame);
}

void loop() {
  readSensor(SN);
  readSensor(WE);
  tlTask();
}
