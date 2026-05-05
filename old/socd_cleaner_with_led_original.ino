#include <EEPROM.h>

const uint8_t eepromAddress = 0;

const uint8_t upOUT = 2; // to button/switch
const uint8_t downOUT = 3; // to button/switch
const uint8_t leftOUT = 4; // to button/switch
const uint8_t rightOUT = 5; // to button/switch

const uint8_t upIN = 6; // from pcb
const uint8_t downIN = 7; // from pcb
const uint8_t leftIN = 8; // from pcb
const uint8_t rightIN = 9; // from pcb

const uint8_t socdLedRED = 10;
const uint8_t socdLedGREEN = 16;
const uint8_t socdLedBLUE = 14;
const uint8_t socdFourLED = 15;

uint8_t leftRead;
uint8_t downRead;
uint8_t upRead;
uint8_t rightRead;

unsigned long currentMillis;
unsigned long upPreviousMillis = 0;
unsigned long downPreviousMillis = 0;
unsigned long leftPreviousMillis = 0;
unsigned long rightPreviousMillis = 0;

uint8_t upPreviousState = 0;
uint8_t downPreviousState = 0;
uint8_t leftPreviousState = 0;
uint8_t rightPreviousState = 0;

uint8_t mode;

void setup() {
  // Serial.begin(9600);

  pinMode(leftOUT, INPUT_PULLUP);
  pinMode(rightOUT, INPUT_PULLUP);
  pinMode(upOUT, INPUT_PULLUP);
  pinMode(downOUT, INPUT_PULLUP);

  pinMode(leftIN, INPUT);
  pinMode(rightIN, INPUT);
  pinMode(downIN, INPUT);
  pinMode(upIN, INPUT);

  pinMode(socdLedRED, OUTPUT);
  pinMode(socdLedGREEN, OUTPUT);
  pinMode(socdLedBLUE, OUTPUT);

  leftRead = digitalRead(leftOUT);
  rightRead = digitalRead(rightOUT);
  upRead = digitalRead(upOUT);
  downRead = digitalRead(downOUT);

  // so !variable means button pressed
  // if (leftRead && rightRead)  mode = 3; // default
  // if (!leftRead && rightRead) mode = 1; // left held down
  // if (leftRead && !rightRead) mode = 2; // right held down
  // if (!leftRead && !rightRead) mode = 0; // left & right held down

  if (leftRead && rightRead && upRead && downRead) mode = EEPROM.read(eepromAddress); // default - read from eeprom
  if (!leftRead && rightRead && upRead && downRead) // left held down
  {
    mode = 0;
    EEPROM.write(eepromAddress, mode);
  } 
  if (!rightRead && leftRead && upRead && downRead) // right held down
  {
    mode = 1;
    EEPROM.write(eepromAddress, mode);
  }
  if (!upRead && rightRead && leftRead && downRead) // up held down
  {
    mode = 2;
    EEPROM.write(eepromAddress, mode);
  }
  if (!downRead && rightRead && upRead && leftRead) // down held down
  {
    mode = 3;
    EEPROM.write(eepromAddress, mode);
  }

  if (mode == 0)
  {
    digitalWrite(socdLedRED, LOW);
    digitalWrite(socdLedGREEN, HIGH);
    digitalWrite(socdLedBLUE, HIGH);
  }
  if (mode == 1)
  {
    digitalWrite(socdLedRED, LOW);
    digitalWrite(socdLedGREEN, HIGH);
    digitalWrite(socdLedBLUE, LOW);
  }
  if (mode == 2)
  {
    digitalWrite(socdLedRED, HIGH);
    digitalWrite(socdLedGREEN, LOW);
    digitalWrite(socdLedBLUE, HIGH);
  }
  if (mode == 3)
  {
    digitalWrite(socdLedRED, HIGH);
    digitalWrite(socdLedGREEN, HIGH);
    digitalWrite(socdLedBLUE, LOW);
  }
}

void loop() {
  currentMillis = millis();

  // Serial.println(mode);

  // 0 is down, 1 is up (0 = true)
  upRead = digitalRead(upOUT);
  downRead = digitalRead(downOUT);
  leftRead = digitalRead(leftOUT);
  rightRead = digitalRead(rightOUT);

  switch (mode)
  {
    case 0:
      outputSOCD0(); // outputRAW
      break;

    case 1:
      outputSOCD1(); // outputSODC
      break;

    case 2:
      outputSOCD2(); // outputLASTCOMMAND
      break;

    case 3:
      outputSOCD3(); // outputLASTCOMMANDnSODC
      break;

    default:
      mode = 0;
      break;
  }
}

// raw output, but for dualsense: left + right = neutral; up + down = neutral
void outputSOCD0() { // outputRAW
  if (!upRead)    pinMode(upIN, OUTPUT);
  digitalWrite(upIN, LOW);
  if (upRead)     pinMode(upIN, INPUT);

  if (!downRead)  pinMode(downIN, OUTPUT);
  digitalWrite(downIN, LOW);
  if (downRead)   pinMode(downIN, INPUT);

  if (!leftRead)  pinMode(leftIN, OUTPUT);
  digitalWrite(leftIN, LOW);
  if (leftRead)   pinMode(leftIN, INPUT);

  if (!rightRead) pinMode(rightIN, OUTPUT);
  digitalWrite(rightIN, LOW);
  if (rightRead)  pinMode(rightIN, INPUT);
}

// left + right = neutral; down + up = up
void outputSOCD1() { // outputSODC
  if (!leftRead)
  {
    if (rightRead)
    {
      pinMode(rightIN, INPUT);
      pinMode(leftIN, OUTPUT);
      digitalWrite(leftIN, LOW);
    }
    else
    {
      pinMode(rightIN, INPUT);
      pinMode(leftIN, INPUT);
    }
  }
  else if (!rightRead)
  {
    pinMode(leftIN, INPUT);
    pinMode(rightIN, OUTPUT);
    digitalWrite(rightIN, LOW);
  }
  else
  {
    pinMode(rightIN, INPUT);
    pinMode(leftIN, INPUT);
  }


  if (!upRead)
  {
    pinMode(downIN, INPUT);
    pinMode(upIN, OUTPUT);
    digitalWrite(upIN, LOW);
  }
  else if (!downRead)
  {
    {
      pinMode(downIN, OUTPUT);
      digitalWrite(downIN, LOW);
      pinMode(upIN, INPUT);
    }
  }
  else
  {
    pinMode(upIN, INPUT);
    pinMode(downIN, INPUT);
  }

}

// left + right = last command wins; up + down = last command wins
void outputSOCD2() { // outputLASTCOMMAND
  if (!upRead) {
    if (!upPreviousState) {
      upPreviousMillis = currentMillis;
      upPreviousState = HIGH;
    }

    if (!downRead) {
      if (upPreviousMillis > downPreviousMillis) {
        pinMode(upIN, OUTPUT);
        digitalWrite(upIN, LOW);
      }
      else
      {
        pinMode(upIN, INPUT);
      }

    }
    else
    {
      pinMode(upIN, OUTPUT);
      digitalWrite(upIN, LOW);
    }
  }


  if (upRead) {
    upPreviousState = LOW;
    pinMode(upIN, INPUT);

  }

  if (!downRead) {
    if (!downPreviousState) {
      downPreviousMillis = currentMillis;
      downPreviousState = HIGH;
    }

    if (!upRead) {
      if (downPreviousMillis > upPreviousMillis) {
        pinMode(downIN, OUTPUT);
        digitalWrite(downIN, LOW);
      }
      else
      {
        pinMode(downIN, INPUT);
      }

    }
    else
    {
      pinMode(downIN, OUTPUT);
      digitalWrite(downIN, LOW);
    }
  }


  if (downRead) {
    downPreviousState = LOW;
    pinMode(downIN, INPUT);

  }

  if (!leftRead) {
    if (!leftPreviousState) {
      leftPreviousMillis = currentMillis;
      leftPreviousState = HIGH;
    }

    if (!rightRead) {
      if (leftPreviousMillis > rightPreviousMillis) {
       
        pinMode(leftIN, OUTPUT);
        digitalWrite(leftIN, LOW); 
      }
      else
      {
        pinMode(leftIN, INPUT);
      }

    }
    else
    {
     pinMode(leftIN, OUTPUT);
        digitalWrite(leftIN, LOW); 
      }
  }


  if (leftRead) {
    leftPreviousState = LOW;
    pinMode(leftIN, INPUT);
    

  }
  if (!rightRead) {
    if (!rightPreviousState) {
      rightPreviousMillis = currentMillis;
      rightPreviousState = HIGH;
    }

    if (!leftRead) {
      if (rightPreviousMillis > leftPreviousMillis) {
       pinMode(rightIN, OUTPUT);
        digitalWrite(rightIN, LOW);
      }
      else
      {
        pinMode(rightIN, INPUT);
      }

    }
    else
    {
      pinMode(rightIN, OUTPUT);
      digitalWrite(rightIN, LOW);
    }
  }

  if (rightRead) {
    rightPreviousState = LOW;
    pinMode(rightIN, INPUT);

  }
}

// up + down = up ; left + right = last command wins
void outputSOCD3() { // outputLASTCOMMANDnSODC
  if (!upRead)
  {
    pinMode(downIN, INPUT);
    pinMode(upIN, OUTPUT);
    digitalWrite(upIN, LOW);
  }
  else if (!downRead)
  {
    {
      pinMode(downIN, OUTPUT);
      digitalWrite(downIN, LOW);
      pinMode(upIN, INPUT);
    }
  }
  else
  {
    pinMode(upIN, INPUT);
    pinMode(downIN, INPUT);
  }


 if (!leftRead) {
    if (!leftPreviousState) {
      leftPreviousMillis = currentMillis;
      leftPreviousState = HIGH;
    }
    if (!rightRead) {
      if (leftPreviousMillis > rightPreviousMillis) {
       
        pinMode(leftIN, OUTPUT);
        digitalWrite(leftIN, LOW); 
      }
      else
      {
        pinMode(leftIN, INPUT);
      }

    }
    else
    {
     pinMode(leftIN, OUTPUT);
        digitalWrite(leftIN, LOW); 
      }
  }


  if (leftRead) {
    leftPreviousState = LOW;
    pinMode(leftIN, INPUT);
    

  }

  if (!rightRead) {
    if (!rightPreviousState) {
      rightPreviousMillis = currentMillis;
      rightPreviousState = HIGH;
    }
    if (!leftRead) {
      if (rightPreviousMillis > leftPreviousMillis) {
       pinMode(rightIN, OUTPUT);
        digitalWrite(rightIN, LOW);
      }
      else
      {
        pinMode(rightIN, INPUT);
      }

    }
    else
    {
      pinMode(rightIN, OUTPUT);
      digitalWrite(rightIN, LOW);
    }
  }

  if (rightRead) {
    rightPreviousState = LOW;
    pinMode(rightIN, INPUT);

  }
}
