#include <Arduino.h>

#include "global_vars.h"

void useSOCD0() {  // left held down - SOCD-N: left + right = neutral; up + down = neutral
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
    pinMode(rightOUT, INPUT);
    pinMode(leftOUT, INPUT);
  }

  if (!upRead) {
    if (downRead) {
      pinMode(downOUT, INPUT);
      pinMode(upOUT, OUTPUT);
      digitalWrite(upOUT, LOW);
    } else {
      pinMode(upOUT, INPUT);
      pinMode(downOUT, INPUT);
    }
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
    SOCD_speech_text = "U+D:N";
  } else if ((!leftRead) && (!rightRead) && ((!upRead) || (!downRead))) {
    SOCD_speech_text = "L+R:N";
  } else if (((!leftRead) || (!rightRead)) && (!upRead) && (!downRead)) {
    SOCD_speech_text = "U+D:N";
  } else {
    SOCD_speech_text = " ";
  }
}
