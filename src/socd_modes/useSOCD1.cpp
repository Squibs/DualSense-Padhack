#pragma once
#include <Arduino.h>

#include "global_vars.h"

// down held down - SOCD-U: left + right = neutral; down + up = up
void useSOCD1() {
  if (!leftRead) {
    if (rightRead) {
      pinMode(rightOUT, INPUT);
      pinMode(leftOUT, OUTPUT);
      digitalWrite(leftOUT, LOW);
    } else {
      pinMode(rightOUT, INPUT);
      pinMode(leftOUT, INPUT);
    }
  } else if (!rightRead) {
    pinMode(leftOUT, INPUT);
    pinMode(rightOUT, OUTPUT);
    digitalWrite(rightOUT, LOW);
  } else {
    pinMode(leftOUT, INPUT);
    pinMode(rightOUT, INPUT);
  }

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

  if ((!leftRead) && (!rightRead) && (!upRead) && (!downRead)) {
    SOCD_speech_text = " WTF";
  } else if ((!leftRead) && (!rightRead) && (upRead) && (downRead)) {
    SOCD_speech_text = "L+R:N";
  } else if ((leftRead) && (rightRead) && (!upRead) && (!downRead)) {
    SOCD_speech_text = "U+D:U";
  } else if ((!leftRead) && (!rightRead) && ((!upRead) || (!downRead))) {
    SOCD_speech_text = "L+R:N";
  } else if (((!leftRead) || (!rightRead)) && (!upRead) && (!downRead)) {
    SOCD_speech_text = "U+D:U";
  } else {
    SOCD_speech_text = " ";
  }
}
