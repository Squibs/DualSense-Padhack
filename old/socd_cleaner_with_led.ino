#include <EEPROM.h>

const uint8_t eepromAddress = 0;

const uint8_t upIN = 2;
const uint8_t downIN = 3;
const uint8_t leftIN = 4;
const uint8_t rightIN = 5;

const uint8_t upOUT = 6;
const uint8_t downOUT = 7;
const uint8_t leftOUT = 8;
const uint8_t rightOUT = 9;

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

void setup()
{
  // Serial.begin(9600);

  pinMode(leftIN, INPUT_PULLUP);
  pinMode(rightIN, INPUT_PULLUP);
  pinMode(upIN, INPUT_PULLUP);
  pinMode(downIN, INPUT_PULLUP);

  pinMode(leftOUT, INPUT);
  pinMode(rightOUT, INPUT);
  pinMode(downOUT, INPUT);
  pinMode(upOUT, INPUT);

  pinMode(socdLedRED, OUTPUT);
  pinMode(socdLedGREEN, OUTPUT);
  pinMode(socdLedBLUE, OUTPUT);

  leftRead = digitalRead(leftIN);
  rightRead = digitalRead(rightIN);
  upRead = digitalRead(upIN);
  downRead = digitalRead(downIN);

  // so !variable means button pressed
  // if (leftRead && rightRead)  mode = 3; // default
  // if (!leftRead && rightRead) mode = 1; // left held down
  // if (leftRead && !rightRead) mode = 2; // right held down
  // if (!leftRead && !rightRead) mode = 0; // left & right held down

  if (leftRead && rightRead && upRead && downRead)
    mode = EEPROM.read(eepromAddress);              // default - read from eeprom
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

void loop()
{
  currentMillis = millis();

  // Serial.println(mode);

  // 0 is down, 1 is up (0 = true)
  upRead = digitalRead(upIN);
  downRead = digitalRead(downIN);
  leftRead = digitalRead(leftIN);
  rightRead = digitalRead(rightIN);

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
void outputSOCD0()
{ // outputRAW
  if (!upRead)
    pinMode(upOUT, OUTPUT);
  digitalWrite(upOUT, LOW);
  if (upRead)
    pinMode(upOUT, INPUT);

  if (!downRead)
    pinMode(downOUT, OUTPUT);
  digitalWrite(downOUT, LOW);
  if (downRead)
    pinMode(downOUT, INPUT);

  if (!leftRead)
    pinMode(leftOUT, OUTPUT);
  digitalWrite(leftOUT, LOW);
  if (leftRead)
    pinMode(leftOUT, INPUT);

  if (!rightRead)
    pinMode(rightOUT, OUTPUT);
  digitalWrite(rightOUT, LOW);
  if (rightRead)
    pinMode(rightOUT, INPUT);
}

// left + right = neutral; down + up = up
void outputSOCD1()
{ // outputSODC
  if (!leftRead)
  {
    if (rightRead)
    {
      pinMode(rightOUT, INPUT);
      pinMode(leftOUT, OUTPUT);
      digitalWrite(leftOUT, LOW);
    }
    else
    {
      pinMode(rightOUT, INPUT);
      pinMode(leftOUT, INPUT);
    }
  }
  else if (!rightRead)
  {
    pinMode(leftOUT, INPUT);
    pinMode(rightOUT, OUTPUT);
    digitalWrite(rightOUT, LOW);
  }
  else
  {
    pinMode(rightOUT, INPUT);
    pinMode(leftOUT, INPUT);
  }

  if (!upRead)
  {
    pinMode(downOUT, INPUT);
    pinMode(upOUT, OUTPUT);
    digitalWrite(upOUT, LOW);
  }
  else if (!downRead)
  {
    {
      pinMode(downOUT, OUTPUT);
      digitalWrite(downOUT, LOW);
      pinMode(upOUT, INPUT);
    }
  }
  else
  {
    pinMode(upOUT, INPUT);
    pinMode(downOUT, INPUT);
  }
}

// left + right = last command wins; up + down = last command wins
void outputSOCD2()
{ // outputLASTCOMMAND
  if (!upRead)
  {
    if (!upPreviousState)
    {
      upPreviousMillis = currentMillis;
      upPreviousState = HIGH;
    }

    if (!downRead)
    {
      if (upPreviousMillis > downPreviousMillis)
      {
        pinMode(upOUT, OUTPUT);
        digitalWrite(upOUT, LOW);
      }
      else
      {
        pinMode(upOUT, INPUT);
      }
    }
    else
    {
      pinMode(upOUT, OUTPUT);
      digitalWrite(upOUT, LOW);
    }
  }

  if (upRead)
  {
    upPreviousState = LOW;
    pinMode(upOUT, INPUT);
  }

  if (!downRead)
  {
    if (!downPreviousState)
    {
      downPreviousMillis = currentMillis;
      downPreviousState = HIGH;
    }

    if (!upRead)
    {
      if (downPreviousMillis > upPreviousMillis)
      {
        pinMode(downOUT, OUTPUT);
        digitalWrite(downOUT, LOW);
      }
      else
      {
        pinMode(downOUT, INPUT);
      }
    }
    else
    {
      pinMode(downOUT, OUTPUT);
      digitalWrite(downOUT, LOW);
    }
  }

  if (downRead)
  {
    downPreviousState = LOW;
    pinMode(downOUT, INPUT);
  }

  if (!leftRead)
  {
    if (!leftPreviousState)
    {
      leftPreviousMillis = currentMillis;
      leftPreviousState = HIGH;
    }

    if (!rightRead)
    {
      if (leftPreviousMillis > rightPreviousMillis)
      {

        pinMode(leftOUT, OUTPUT);
        digitalWrite(leftOUT, LOW);
      }
      else
      {
        pinMode(leftOUT, INPUT);
      }
    }
    else
    {
      pinMode(leftOUT, OUTPUT);
      digitalWrite(leftOUT, LOW);
    }
  }

  if (leftRead)
  {
    leftPreviousState = LOW;
    pinMode(leftOUT, INPUT);
  }
  if (!rightRead)
  {
    if (!rightPreviousState)
    {
      rightPreviousMillis = currentMillis;
      rightPreviousState = HIGH;
    }

    if (!leftRead)
    {
      if (rightPreviousMillis > leftPreviousMillis)
      {
        pinMode(rightOUT, OUTPUT);
        digitalWrite(rightOUT, LOW);
      }
      else
      {
        pinMode(rightOUT, INPUT);
      }
    }
    else
    {
      pinMode(rightOUT, OUTPUT);
      digitalWrite(rightOUT, LOW);
    }
  }

  if (rightRead)
  {
    rightPreviousState = LOW;
    pinMode(rightOUT, INPUT);
  }
}

// up + down = up ; left + right = last command wins
void outputSOCD3()
{ // outputLASTCOMMANDnSODC
  if (!upRead)
  {
    pinMode(downOUT, INPUT);
    pinMode(upOUT, OUTPUT);
    digitalWrite(upOUT, LOW);
  }
  else if (!downRead)
  {
    {
      pinMode(downOUT, OUTPUT);
      digitalWrite(downOUT, LOW);
      pinMode(upOUT, INPUT);
    }
  }
  else
  {
    pinMode(upOUT, INPUT);
    pinMode(downOUT, INPUT);
  }

  if (!leftRead)
  {
    if (!leftPreviousState)
    {
      leftPreviousMillis = currentMillis;
      leftPreviousState = HIGH;
    }
    if (!rightRead)
    {
      if (leftPreviousMillis > rightPreviousMillis)
      {

        pinMode(leftOUT, OUTPUT);
        digitalWrite(leftOUT, LOW);
      }
      else
      {
        pinMode(leftOUT, INPUT);
      }
    }
    else
    {
      pinMode(leftOUT, OUTPUT);
      digitalWrite(leftOUT, LOW);
    }
  }

  if (leftRead)
  {
    leftPreviousState = LOW;
    pinMode(leftOUT, INPUT);
  }

  if (!rightRead)
  {
    if (!rightPreviousState)
    {
      rightPreviousMillis = currentMillis;
      rightPreviousState = HIGH;
    }
    if (!leftRead)
    {
      if (rightPreviousMillis > leftPreviousMillis)
      {
        pinMode(rightOUT, OUTPUT);
        digitalWrite(rightOUT, LOW);
      }
      else
      {
        pinMode(rightOUT, INPUT);
      }
    }
    else
    {
      pinMode(rightOUT, OUTPUT);
      digitalWrite(rightOUT, LOW);
    }
  }

  if (rightRead)
  {
    rightPreviousState = LOW;
    pinMode(rightOUT, INPUT);
  }
}
