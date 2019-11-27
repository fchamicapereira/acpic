#include <Wire.h>

/* * * * * * * * * * * * * *
 *  
 *  ADJUST THESE PARAMETERS
 *  
 * * * * * * * * * * * * * */

#define MAX_X 2
#define MAX_Y 2

byte x = 0;
byte y = 0;

byte s = 1;
byte w = 1;

/* * * * * * * * * * * * * *
 *  
 *  Other constants
 *  
 * * * * * * * * * * * * * */
 
#define N_TRAFFIC_LIGHTS 2

#define EPOCH_UNIT 100 // 0.1 seconds

// time that takes one car to go from one intersections to the other
#define TRAVEL_DISTANCE 6000

#define R2G_N 0
#define R2G_S 1
#define R2G_E 2
#define R2G_W 3

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

const int X_INPUT = A0;

int CARS[N_TRAFFIC_LIGHTS] = { 0, 0 };

state tlState = INITIAL;

struct green_wave {
  bool active;
  bool adjusted;
  byte dir;
  long int t;
};

struct green_wave pending_adjustment;

/* * * * * * * * * * * * * *
 *  
 *  End of configuration
 *  
 * * * * * * * * * * * * * */

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
      normal2Task();
      break;
  }
}

void initialTask(int tl) {
  static unsigned long delay[N_TRAFFIC_LIGHTS] = { millis(), millis() };
  
  unsigned long dt = millis() - delay[tl];
  unsigned long globalDt = millis();
  
  int light = TL[tl][YELLOW];

  if (globalDt > INITIAL_TASK_DURATION) {
    tlState = NORMAL2;
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

  CARS[SN] = 0; // reset car counter
  CARS[WE] = 0; // reset car counter
}

void normal2Task() {
  // used for one time broadcasting for each state
  static byte state = 0xff;
  
  static unsigned long epoch_counter = millis();
  static unsigned long epoch = 0;
  
  static unsigned long delay = millis();
  unsigned long dt = millis() - delay;
  
  static float duty_cycle = 0.5;

  if (millis() - epoch_counter > EPOCH_UNIT) {
    epoch++; epoch_counter = millis();
  }
  
  if (dt < NORMAL1_TASK_PERIOD * duty_cycle - TRANSITION_TIME) {
    if (state != 0) {
      state = 0;
      broadcast(epoch, s == 1 ? R2G_S : R2G_N);
    }

    if (pending_adjustment.active && pending_adjustment.dir == SN
      && (abs(pending_adjustment.t - epoch) / 10) %  NORMAL1_TASK_PERIOD >= TRAVEL_DISTANCE - TRANSITION_TIME
      && (abs(pending_adjustment.t - epoch) / 10) %  NORMAL1_TASK_PERIOD <= TRAVEL_DISTANCE + TRANSITION_TIME) {
      pending_adjustment.active = false;
     }

    if (pending_adjustment.active && !pending_adjustment.adjusted) {
      delay += TRANSITION_TIME;
      pending_adjustment.adjusted = true;
    }
    
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
    if (state != 1) {
      state = 1;
      broadcast(epoch, w == 1 ? R2G_W : R2G_E);
    }

    if (pending_adjustment.active && pending_adjustment.dir == WE
      && (abs(pending_adjustment.t - epoch) / 10) %  NORMAL1_TASK_PERIOD >= TRAVEL_DISTANCE - TRANSITION_TIME
      && (abs(pending_adjustment.t - epoch) / 10) %  NORMAL1_TASK_PERIOD <= TRAVEL_DISTANCE + TRANSITION_TIME) {
      pending_adjustment.active = false;
    }  

    if (pending_adjustment.active && !pending_adjustment.adjusted) {
      delay += TRANSITION_TIME;
      pending_adjustment.adjusted = true;
    }
    
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

  CARS[SN] = 0; // reset car counter
  CARS[WE] = 0; // reset car counter

  pending_adjustment.adjusted = false;
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

  int x_value = analogRead(X_INPUT);
  int ds = 1024.0 / 8;
  for (int i = 0; i < 8; i++) {
    Serial.print(i);
    Serial.print(" ");
    Serial.print(ds * i);
    Serial.print(" => ");
    Serial.println(ds * (i + 1) - 1);
  }
  
  Serial.println(x_value);
  Serial.println(x_value * 1.0 / ds);

  // join the I2C bus
  Wire.begin();
  Wire.onReceive(receiveFrame);

  pending_adjustment.active = false;
}

void receiveFrame(int howMany) {
  frame f;
  
  byte dst_x, dst_y;
  byte src_x, src_y;
  byte rel_x, rel_y;
  
  byte sender_max_flow;
  byte local_max_flow;
  
  while (Wire.available() > 0) {
    Wire.readBytes((byte*)&f, sizeof(frame));

    // grab the least significant 4 bits
    dst_x = f.dst & 15;
    
    // grab the other 4 bits
    dst_y = f.dst >> 4;

    // check if the message is addressed to this controller
    if (dst_x != x || dst_y != y) return;

    // grab the least significant 4 bits
    src_x = f.src & 15;
    
    // grab the other 4 bits
    src_y = f.src >> 4;

    // my position relative to the sender
    rel_x = src_x - x;
    rel_y = src_y - y;

    sender_max_flow = max(max(max(f.cars.n, f.cars.s), f.cars.e), f.cars.w);
    local_max_flow = max(CARS[SN], CARS[WE]);

    // sender is further north
    // max traffic flow only relevant if comming from the north
    if (rel_x == 0 && rel_y > 0 && f.cars.n == sender_max_flow && CARS[SN] == local_max_flow) {
      pending_adjustment.active = true;
      pending_adjustment.t = f.t;
      pending_adjustment.adjusted = false;
      pending_adjustment.dir = SN;
    }
    
    // sender is further south
    // max traffic flow only relevant if comming from the south
    else if (rel_x == 0 && rel_y < 0 && f.cars.s == sender_max_flow && CARS[SN] == local_max_flow) {
      pending_adjustment.active = true;
      pending_adjustment.t = f.t;
      pending_adjustment.adjusted = false;
      pending_adjustment.dir = SN;
    }

  
    // sender is further west
    // max traffic flow only relevant if comming from the west
    else if (rel_x < 0 && rel_y == 0 && f.cars.w == sender_max_flow && CARS[WE] == local_max_flow) {
      pending_adjustment.active = true;
      pending_adjustment.t = f.t;
      pending_adjustment.adjusted = false;
      pending_adjustment.dir = WE;
    }

    // sender is further east
    // max traffic flow only relevant if comming from the east
    else if (rel_x > 0 && rel_y == 0 && f.cars.e == sender_max_flow && CARS[WE] == local_max_flow) {
      pending_adjustment.active = true;
      pending_adjustment.t = f.t;
      pending_adjustment.adjusted = false;
      pending_adjustment.dir = WE;
    }
    
    Serial.print("READ ");
    Serial.print(f.dst);
    Serial.print(" ");
    Serial.print(f.src);
    Serial.print(" ");
    Serial.print(f.event);
    Serial.print(" ");
    Serial.print(f.cars.n);
    Serial.print(" ");
    Serial.print(f.cars.e);
    Serial.print(" ");
    Serial.print(f.cars.s);
    Serial.print(" ");
    Serial.print(f.cars.w);
    Serial.print(" ");
    Serial.println(f.t);
  }
}

void broadcast(unsigned long epoch, byte action) {  
  if (x-1 >= 0)     send(epoch, action, x-1, y);
  if (x+1 <= MAX_X) send(epoch, action, x+1, y);
  if (y-1 >= 0)     send(epoch, action, x, y-1);
  if (y+1 <= MAX_Y) send(epoch, action, x, y+1);
}

void send(unsigned long epoch, byte action, byte dst_x, byte dst_y) {
  while (Wire.available()) { delay(10); }
  
  frame f;
  
  f.dst = dst_y << 4 + dst_x; // 0 dst_x2 dst_x1 dst_x0 0 dst_y2 dst_y1 dst_y0
  f.src = y << 4 + x; // 0 x2 x1 x0 0 y2 y1 y0
  
  f.event = action;
  
  f.cars.n = s == 0 ? CARS[SN] : 0;
  f.cars.s = s == 1 ? CARS[SN] : 0;
  f.cars.e = w == 0 ? CARS[WE] : 0;
  f.cars.w = w == 1 ? CARS[WE] : 0;
  
  f.t = epoch;

  Serial.println("WRITE");
  
  Wire.beginTransmission(-1);
  Wire.write((byte*)&f, sizeof(frame));
  Wire.endTransmission();
}

void loop() {
  readSensor(SN);
  readSensor(WE);
  tlTask();
}
