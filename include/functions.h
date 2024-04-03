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
void controlButtonVisualRenderHelper(uint8_t, bool);
void drawArt(uint8_t, uint8_t, const std::vector<std::vector<int>>& artToDraw);
