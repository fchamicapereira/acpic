const int BUTTON = 6;
const int YELLOW = 2;
const int BLUE = 3;
const int RED = 4;
const int GREEN = 5;
const unsigned long dt = 1000; // 1 second

short ledState = 0;
int buttonInput = LOW;
int buttonState = 0;
bool freeze = false;

void stepLedState() {
  switch (ledState) {
    case 0:
      digitalWrite(RED, HIGH);
      break;
    case 1:
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, HIGH);
      break;
    case 2:
      digitalWrite(GREEN, LOW);
      digitalWrite(BLUE, HIGH);
      break;
    case 3:
      digitalWrite(BLUE, LOW);
      digitalWrite(YELLOW, HIGH);
      break;
    case 4:
      digitalWrite(YELLOW, LOW);
      break;
    default:
      break;
  }

  ledState = (ledState + 1) % 5;
}

void stepButtonState(int buttonIntput) {
  switch (buttonState) {
    case 0:
      if (buttonInput == HIGH) {
        freeze = true;
        buttonState += 1;
      }
      break;
    case 1:
      if (buttonInput == LOW) buttonState = 2;
      break;
    case 2:
      if (buttonInput == HIGH) {
        buttonState = 3;
        freeze = false;
      }
      break;
    case 3:
      if (buttonInput == LOW) {
        buttonState = 0;
      }
      break;
  }
}

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  
  pinMode(BUTTON, INPUT);
}

void loop() { 
  buttonInput = digitalRead(BUTTON);
  stepButtonState(buttonInput);

  if (!freeze) {
    stepLedState();
    delay(dt);
  }
}
