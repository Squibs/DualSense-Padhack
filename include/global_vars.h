#pragma once
#include <Arduino.h>

extern uint8_t leftRead;
extern uint8_t downRead;
extern uint8_t upRead;
extern uint8_t rightRead;

extern const uint8_t upOUT;
extern const uint8_t downOUT;
extern const uint8_t leftOUT;
extern const uint8_t rightOUT;

extern unsigned long currentMillis;
extern unsigned long upPreviousMillis;
extern unsigned long downPreviousMillis;
extern unsigned long leftPreviousMillis;
extern unsigned long rightPreviousMillis;

extern uint8_t upPreviousState;
extern uint8_t downPreviousState;
extern uint8_t leftPreviousState;
extern uint8_t rightPreviousState;

extern String SOCD_speech_text;
