#include <Arduino.h>

#include "global_vars.h"

// up held down - SOCD-LU: left + right = last command wins; up + down = up
void useSOCD3() {
  if (!upRead) {
    pinMode(downOUT, INPUT);
    pinMode(upOUT, OUTPUT);
    digitalWrite(upOUT, LOW);
  } else if (!downRead) {
    pinMode(upOUT, INPUT);
    pinMode(downOUT, OUTPUT);
    digitalWrite(downOUT, LOW);
  } else {
    pinMode(upOUT, INPUT);
    pinMode(downOUT, INPUT);
  }

  if (!leftRead) {
    if (!leftPreviousState) {
      leftPreviousMillis = currentMillis;
      leftPreviousState = 1;
    }
    if (!rightRead) {
      if (leftPreviousMillis > rightPreviousMillis) {
        pinMode(leftOUT, OUTPUT);
        digitalWrite(leftOUT, INPUT);
      } else {
        pinMode(leftOUT, INPUT);
      }
    } else {
      pinMode(leftOUT, OUTPUT);
      digitalWrite(leftOUT, LOW);
    }
  }

  if (leftRead) {
    leftPreviousState = 0;
    pinMode(leftOUT, INPUT);
  }

  if (!rightRead) {
    if (!rightPreviousState) {
      rightPreviousMillis = currentMillis;
      rightPreviousState = 1;
    }
    if (!leftRead) {
      if (rightPreviousMillis > leftPreviousMillis) {
        pinMode(rightOUT, OUTPUT);
        digitalWrite(rightOUT, LOW);
      } else {
        pinMode(rightOUT, INPUT);
      }
    } else {
      pinMode(rightOUT, OUTPUT);
      digitalWrite(rightOUT, LOW);
    }
  }

  if (rightRead) {
    rightPreviousState = 0;
    pinMode(rightOUT, INPUT);
  }

  if ((!leftRead) && (!rightRead) && (!upRead) && (!downRead)) {
    SOCD_speech_text = " WTF";
  } else if ((!leftRead) && (!rightRead) && (upRead) && (downRead) && (leftPreviousMillis > rightPreviousMillis)) {
    SOCD_speech_text = "L+R:L";
  } else if ((!leftRead) && (!rightRead) && (upRead) && (downRead) && (leftPreviousMillis < rightPreviousMillis)) {
    SOCD_speech_text = "L+R:R";
  } else if ((!upRead) && (!downRead) && (leftRead) && (rightRead)) {
    SOCD_speech_text = "U+D:U";
  } else if ((!leftRead) && (!rightRead) && (!upRead || !downRead) && leftPreviousMillis > rightPreviousMillis) {
    SOCD_speech_text = "L+R:L";
  } else if ((!leftRead) && (!rightRead) && (!upRead || !downRead) && leftPreviousMillis < rightPreviousMillis) {
    SOCD_speech_text = "L+R:R";
  } else if ((!upRead) && (!downRead) && (!leftRead || !rightRead)) {
    SOCD_speech_text = "U+D:U";
  } else {
    SOCD_speech_text = " ";
  }
}
