#include <Arduino.h>

#include "global_vars.h"

// right held down - SOCD-L: left + right = last command wins; up + down = last command wins
void useSOCD2() {
  if (!upRead) {
    if (!upPreviousState) {
      upPreviousMillis = currentMillis;
      upPreviousState = 1;
    }
    if (!downRead) {
      if (upPreviousMillis > downPreviousMillis) {
        pinMode(upOUT, OUTPUT);
        digitalWrite(upOUT, LOW);
      } else {
        pinMode(upOUT, INPUT);
      }
    } else {
      pinMode(upOUT, OUTPUT);
      digitalWrite(upOUT, LOW);
    }
  }

  if (upRead) {
    upPreviousState = 0;
    pinMode(upOUT, INPUT);
  }

  if (!downRead) {
    if (!downPreviousState) {
      downPreviousMillis = currentMillis;
      downPreviousState = 1;
    }
    if (!upRead) {
      if (downPreviousMillis > upPreviousMillis) {
        pinMode(downOUT, OUTPUT);
        digitalWrite(downOUT, LOW);
      } else {
        pinMode(downOUT, INPUT);
      }
    } else {
      pinMode(downOUT, OUTPUT);
      digitalWrite(downOUT, LOW);
    }
  }

  if (downRead) {
    downPreviousState = 0;
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
        digitalWrite(leftOUT, LOW);
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
  } else if ((!upRead) && (!downRead) && (leftRead) && (rightRead) && (downPreviousMillis > upPreviousMillis)) {
    SOCD_speech_text = "U+D:U";
  } else if ((!upRead) && (!downRead) && (leftRead) && (rightRead) && (downPreviousMillis < upPreviousMillis)) {
    SOCD_speech_text = "U+D:D";
  } else if ((!leftRead) && (!rightRead) && (!upRead || !downRead) && leftPreviousMillis > rightPreviousMillis) {
    SOCD_speech_text = "L+R:L";
  } else if ((!leftRead) && (!rightRead) && (!upRead || !downRead) && leftPreviousMillis < rightPreviousMillis) {
    SOCD_speech_text = "L+R:R";
  } else if ((!upRead) && (!downRead) && (!leftRead || !rightRead) && downPreviousMillis > upPreviousMillis) {
    SOCD_speech_text = "U+D:U";
  } else if ((!upRead) && (!downRead) && (!leftRead || !rightRead) && downPreviousMillis > upPreviousMillis) {
    SOCD_speech_text = "U+D:D";
  } else {
    SOCD_speech_text = " ";
  }
}
