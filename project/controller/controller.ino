#include <Wire.h>

#define N_TRAFFIC_LIGHTS 2

#define SN 0
#define WE 1

#define GREEN 0
#define YELLOW 1
#define RED 2

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
const int TL[N_TRAFFIC_LIGHTS][3] = { { 0, 1, 2 }, { 3, 4, 5 } };

const int LD_SN = 12;
const int LD_WE = 13;

state tlState = INITIAL;

// traffic lights' task
void tlTask() {
  static unsigned long delay;
  unsigned long dt = millis() - delay;
  
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
      standByTask(SN);
      standByTask(WE);
      break;
      
    case NORMAL1:
      break;
      
    case NORMAL2:
      break;
  }
}

void initialTask(int tl) {
  static unsigned long delay[N_TRAFFIC_LIGHTS];
  
  unsigned long dt = millis() - delay[tl];
  unsigned long globalDt = millis();
  
  int light = TL[tl][YELLOW];

  if (globalDt > INITIAL_TASK_DURATION) {
    tlState = NORMAL0;
    return;
  }

  if (dt < INITIAL_TASK_PERIOD / 2) digitalWrite(light, HIGH);
  else if (dt > INITIAL_TASK_PERIOD) delay[tl] = millis();
  else digitalWrite(light, LOW);
}

void standByTask(int tl) {
  static unsigned long delay[N_TRAFFIC_LIGHTS];
  unsigned long dt = millis() - delay[tl];
  int light = TL[tl][YELLOW];

  if (dt < STANDBY_TASK_PERIOD / 2) digitalWrite(light, HIGH);
  else if (dt > INITIAL_TASK_PERIOD) delay[tl] = millis();
  else digitalWrite(light, LOW);
}

void normal0Task(int tl) {

  
}

void receiveFrame(int howMany) {
  frame f;
  
  while (Wire.available()) {
    Wire.readBytes((byte*)&f, sizeof(frame));
  }
}

void setup() {
  pinMode(TL[SN][GREEN], OUTPUT);
  pinMode(TL[SN][YELLOW], OUTPUT);
  pinMode(TL[SN][RED], OUTPUT);

  pinMode(TL[WE][GREEN], OUTPUT);
  pinMode(TL[WE][YELLOW], OUTPUT);
  pinMode(TL[WE][RED], OUTPUT);

  pinMode(LD_SN, INPUT);
  pinMode(LD_WE, INPUT);

  // join the I2C bus
  Wire.begin();
  Wire.onReceive(receiveFrame);
}

void loop() {
  tlTask();
}
