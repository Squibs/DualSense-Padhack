#pragma once
#include <Arduino.h>

void drawButtonCircles(uint8_t);
void drawButtonLabels(uint8_t);
void drawSOCDLabels();
void chooseSOCD();
void useSOCD0();
void useSOCD1();
void useSOCD2();
void useSOCD3();
void useSOCD4();
void drawSpeechText(bool);
void drawSquid(String);
void drawCat(String);
void drawSagat(String);
void controlButtonRender();
void controlDirectionalButtonsRender(uint8_t, bool& flag, bool& flag2);
void controlButtonVisualRender(uint8_t, bool& flag, bool& flag2);
void drawButtonCirclesFilled(uint8_t, bool);
void drawArt(uint8_t, uint8_t, const std::vector<std::vector<int>>& artToDraw);
void buttonPressedLogger(arduino::String buttonName, bool color);
void drawProgressBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
