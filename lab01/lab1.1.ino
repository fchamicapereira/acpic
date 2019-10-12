const int BUTTON = 6;
const int YELLOW = 2;
const int BLUE = 3;
const int RED = 4;
const int GREEN = 5;
const unsigned long dt = 1000; // 1 second

int ledState = 1;
int ledState_previous = 0;
bool freeze = false;

int buttonInput;
int buttonInput_previous;
static int buttonState = 0;  //0 off and 1 on
int buttonState_previous;

static int pressButtonNumber = 0;



/*
** Start the program
*/

void setup() {
  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(BUTTON, INPUT);

  buttonState_previous = buttonState;
  buttonInput = LOW;
  stepLedState(ledState);
}

/*
** Workin while the program running
*/
void loop() { 
  
  buttonInput = digitalRead(BUTTON);
  
  
  //Verify if the button was pressed 3 times
  //Missing code
  //pressNumber++;
  /*
  if(buttonInput != buttonInput_previous && buttonInput_previous == HIGH && pressNumber < 3)
  { 
    pressNumber++;
  }
  else if (buttonInput != buttonInput_previous && buttonInput_previous == HIGH && pressNumber > 3)
  {
    pressNumber = 0;
  }
  */
  if(buttonInput == HIGH)
  {
    freeze = !freeze;
  }
  
  
  Serial.print(buttonInput);
  
  if( !freeze )
  {
      stepLedState( ledState );
      delay(dt);
      ledState ++;
      //pressNumber = 0; 
      buttonInput_previous = buttonInput;  
  }
  else //if( buttonInput == HIGH && pressNumber != 3)
  {
    stepLedState(ledState-1);
    delay(dt);
  }
}


/*
** Set the light freeze
*/
void setLightOn(bool turnOnRed, bool turnOnGreen, bool turnOnBlue, bool turnOnYellow)
{

  if(turnOnRed)
  {
    digitalWrite(RED, HIGH);
    digitalWrite(BLUE, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);
  }
  else if(turnOnBlue)
  {
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, HIGH);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);
  }
  else if(turnOnYellow)
  {
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(YELLOW, HIGH);
    digitalWrite(GREEN, LOW);
  }
  else if(turnOnGreen)
  {
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, HIGH);
  }
  //turn all the lights off
  else
  {
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);
    ledState = 0;
  }
}


/*
** Set the light on and turn off the others
*/
void stepLedState(int ledState) {

  switch (ledState){
    case 1:
      setLightOn(true, false, false,false);
      break;
    case 2:
      setLightOn(false, true, false,false);
      break;
    case 3:
      setLightOn(false, false, true,false);
      break;
    case 4:
      setLightOn(false, false, false,true);
      break;
    case 5:
      setLightOn(false, false, false,false);
      break;  
    default:
      break;
  }
}
