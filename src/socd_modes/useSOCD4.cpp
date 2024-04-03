#pragma once
#include <Arduino.h>

#include "global_vars.h"

// up and down held down - SOCD-R: raw output, but on dualsense: left + right = neutral; up + down = neutral
void useSOCD4() {
  if (!upRead) {
    pinMode(upOUT, OUTPUT);
    digitalWrite(upOUT, LOW);
  }
  if (upRead) {
    pinMode(upOUT, INPUT);
  }

  if (!downRead) {
    pinMode(downOUT, OUTPUT);
    digitalWrite(downOUT, LOW);
  }
  if (downRead) {
    pinMode(downOUT, INPUT);
  }

  if (!leftRead) {
    pinMode(leftOUT, OUTPUT);
    digitalWrite(leftOUT, LOW);
  }
  if (leftRead) {
    pinMode(leftOUT, INPUT);
  }

  if (!rightRead) {
    pinMode(rightOUT, OUTPUT);
    digitalWrite(rightOUT, LOW);
  }
  if (rightRead) {
    pinMode(rightOUT, INPUT);
  }

  if (!leftRead && !rightRead && !upRead && !downRead) {
    SOCD_speech_text = " WTF";
  } else {
    SOCD_speech_text = " ";
  }
}
