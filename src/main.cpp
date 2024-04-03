#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Wire.h>

#include <vector>

#include "LittleFS.h"  // LittleFS is declared
#include "art.h"
#include "functions.h"
#include "global_vars.h"
#include "squibs_logo.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
#define OLED_RESET -1                                                               // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C                                                         ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32; 0x3C for my particular display which is 12x64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET, 400000);  // Wire1 for GP 26/27 instead of Wire0 for GP 4/5
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

const uint8_t upIN = 0;
const uint8_t downIN = 1;
const uint8_t leftIN = 2;
const uint8_t rightIN = 3;

const uint8_t upOUT = 4;
const uint8_t downOUT = 5;
const uint8_t leftOUT = 6;
const uint8_t rightOUT = 7;

uint8_t leftRead;
uint8_t downRead;
uint8_t upRead;
uint8_t rightRead;

const uint8_t btnSquare = 8;
const uint8_t btnTriangle = 9;
const uint8_t btnR1 = 10;
const uint8_t btnL1 = 11;
const uint8_t btnX = 12;
const uint8_t btnCircle = 13;
const uint8_t btnR2 = 14;
const uint8_t btnL2 = 15;
const uint8_t btnHome = 16;
const uint8_t btnShare = 17;
const uint8_t btnOptions = 18;
const uint8_t btnR3 = 19;
const uint8_t btnL3 = 20;
const uint8_t btnTouch = 21;
const uint8_t btnChooseSOCD = 22;

bool upFlag = true;
bool downFlag = true;
bool leftFlag = true;
bool rightFlag = true;
bool squareFlag = true;
bool triangleFlag = true;
bool xFlag = true;
bool circleFlag = true;
bool r1Flag = true;
bool r2Flag = true;
bool r3Flag = true;
bool l1Flag = true;
bool l2Flag = true;
bool l3Flag = true;
bool homeFlag = true;
bool shareFlag = true;
bool optionsFlag = true;
bool touchFlag = true;
bool SOCDChangeFlag = true;

bool upFlag2 = false;
bool downFlag2 = false;
bool leftFlag2 = false;
bool rightFlag2 = false;
bool squareFlag2 = false;
bool triangleFlag2 = false;
bool xFlag2 = false;
bool circleFlag2 = false;
bool r1Flag2 = false;
bool r2Flag2 = false;
bool r3Flag2 = false;
bool l1Flag2 = false;
bool l2Flag2 = false;
bool l3Flag2 = false;
bool homeFlag2 = false;
bool shareFlag2 = false;
bool optionsFlag2 = false;
bool touchFlag2 = false;
bool SOCDChangeFlag2 = false;

unsigned long currentMillis;
unsigned long upPreviousMillis = 0;
unsigned long downPreviousMillis = 0;
unsigned long leftPreviousMillis = 0;
unsigned long rightPreviousMillis = 0;

uint8_t upPreviousState = 0;
uint8_t downPreviousState = 0;
uint8_t leftPreviousState = 0;
uint8_t rightPreviousState = 0;

int SOCD_mode;
String SOCD_speech_text = " ";
String SOCD_last_value = " ";
const int art_animation_speed = 43;
int art_animation_control = art_animation_speed;
bool art_animation_flag = true;

void setup() {
  Serial.begin(9600);  // Allows printlns / monitoring

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  u8g2_for_adafruit_gfx.begin(display);

  // Display logo
  display.clearDisplay();
  display.drawBitmap(0, 5, epd_bitmap_Squibs, 128, 64, WHITE);
  display.display();
  delay(2000);  // Pause for 2 seconds

  // set d-pad in pins
  pinMode(leftIN, INPUT_PULLUP);
  pinMode(rightIN, INPUT_PULLUP);
  pinMode(upIN, INPUT_PULLUP);
  pinMode(downIN, INPUT_PULLUP);

  // set d-pad out pins - starts as input for safety
  pinMode(leftOUT, INPUT);
  pinMode(rightOUT, INPUT);
  pinMode(upOUT, INPUT);
  pinMode(downOUT, INPUT);

  // all other button pins
  pinMode(btnSquare, INPUT_PULLUP);
  pinMode(btnTriangle, INPUT_PULLUP);
  pinMode(btnR1, INPUT_PULLUP);
  pinMode(btnL1, INPUT_PULLUP);
  pinMode(btnX, INPUT_PULLUP);
  pinMode(btnCircle, INPUT_PULLUP);
  pinMode(btnR2, INPUT_PULLUP);
  pinMode(btnL2, INPUT_PULLUP);
  pinMode(btnHome, INPUT_PULLUP);
  pinMode(btnShare, INPUT_PULLUP);
  pinMode(btnOptions, INPUT_PULLUP);
  pinMode(btnR3, INPUT_PULLDOWN);  // R3 & L3 are active Highs, opposite of the rest
  pinMode(btnL3, INPUT_PULLDOWN);  // R3 & L3 are active Highs, opposite of the rest
  pinMode(btnTouch, INPUT_PULLUP);
  pinMode(btnChooseSOCD, INPUT_PULLUP);

  // check if a saved socd mode exists, if not: create one
  LittleFS.begin();
  if (!LittleFS.exists("/SOCD_Data.txt")) {
    LittleFS.mkdir("/SOCD_Data.txt");
    File socd_file = LittleFS.open("/SOCD_Data.txt", "w");
    socd_file.println(0);
    socd_file.close();
    SOCD_mode = 0;
  } else {
    // read existing socd mode
    File socd_file = LittleFS.open("/SOCD_Data.txt", "r");
    while (socd_file.available()) {
      SOCD_mode = socd_file.readString().toInt();
    }
    socd_file.close();
  }

  Serial.print("SOCD_mode: ");
  Serial.println(SOCD_mode);

  // initial render of buttons and labels
  display.clearDisplay();
  drawButtonCircles(0);  // all
  drawButtonLabels(0);   // all
  drawSOCDLabels();

  // draw Squibs text: bottom right
  display.setTextSize(1);
  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_ImpactBits_tr);
  u8g2_for_adafruit_gfx.setCursor(85, 63);
  u8g2_for_adafruit_gfx.print("SQUIBS");

  display.display();
}

void loop() {
  // read directional buttons current state
  currentMillis = millis();
  leftRead = digitalRead(leftIN);
  rightRead = digitalRead(rightIN);
  upRead = digitalRead(upIN);
  downRead = digitalRead(downIN);

  // if SOCD change button is pressed
  if (!digitalRead(btnChooseSOCD) && SOCDChangeFlag) {
    chooseSOCD();
    SOCDChangeFlag = false;
    SOCDChangeFlag2 = true;
  } else if (digitalRead(btnChooseSOCD) && SOCDChangeFlag2) {
    SOCDChangeFlag = true;
    SOCDChangeFlag2 = false;
  }

  // use already chosen/current/appropriate SOCD_mode
  if (SOCD_mode == 0) {
    useSOCD0();  // left held down - SOCD-N: left + right = neutral; up + down = neutral
  } else if (SOCD_mode == 1) {
    useSOCD1();  // down held down - SOCD-U: left + right = neutral; down + up = up
  } else if (SOCD_mode == 2) {
    useSOCD2();  // right held down - SOCD-L: left + right = last command wins; up + down = last command wins
  } else if (SOCD_mode == 3) {
    useSOCD3();  // up held down - SOCD-LU: left + right = last command wins; up + down = up
  } else if (SOCD_mode == 4) {
    useSOCD4();  // up and down held down - SOCD-R: raw output, but on dualsense: left + right = neutral; up + down = neutral
  }

  // draw SOCD speech text and bubble if needed
  if (SOCD_last_value != SOCD_speech_text) {
    SOCD_last_value = SOCD_speech_text;
    if (SOCD_speech_text == " ") {
      drawSpeechText(1);
    } else {
      drawSpeechText(1);
      drawSpeechText(0);
    }
  }

  // what to do when a button is pressed:
  // directional buttons
  controlButtonVisualRender(upOUT, upFlag, upFlag2);
  controlButtonVisualRender(downOUT, downFlag, downFlag2);
  controlButtonVisualRender(leftOUT, leftFlag, leftFlag2);
  controlButtonVisualRender(rightOUT, rightFlag, rightFlag2);
  // all other buttons
  controlButtonVisualRender(btnSquare, squareFlag, squareFlag2);
  controlButtonVisualRender(btnTriangle, triangleFlag, triangleFlag2);
  controlButtonVisualRender(btnR1, r1Flag, r1Flag2);
  controlButtonVisualRender(btnL1, l1Flag, l1Flag2);
  controlButtonVisualRender(btnX, xFlag, xFlag2);
  controlButtonVisualRender(btnCircle, circleFlag, circleFlag2);
  controlButtonVisualRender(btnR2, r2Flag, r2Flag2);
  controlButtonVisualRender(btnL2, l2Flag, l2Flag2);
  controlButtonVisualRender(btnHome, homeFlag, homeFlag2);
  controlButtonVisualRender(btnShare, shareFlag, shareFlag2);
  controlButtonVisualRender(btnOptions, optionsFlag, optionsFlag2);
  controlButtonVisualRender(btnR3, r3Flag, r3Flag2);
  controlButtonVisualRender(btnL3, l3Flag, l3Flag2);
  controlButtonVisualRender(btnTouch, touchFlag, touchFlag2);

  // art animation control
  art_animation_control -= 1;
  if (art_animation_control <= 0) {
    art_animation_control = art_animation_speed;
    art_animation_flag = true;
  } else if (art_animation_control == round(art_animation_speed / 2)) {
    art_animation_flag = true;
  }

  if (art_animation_control > round(art_animation_speed / 2) && art_animation_flag) {
    drawArt(0, 49, pirate1);
    drawArt(17, 55, cat1);
    drawArt(30, 48, sagat1);
    drawArt(66, 49, squid1);
  } else if (art_animation_control < round(art_animation_speed / 2) && art_animation_flag) {
    drawArt(0, 49, pirate2);
    drawArt(17, 55, cat2);
    drawArt(30, 48, sagat2);
    drawArt(66, 49, squid2);
  }

  display.display();
}

void saveSOCD(int8_t SOCDToSave) {
  LittleFS.remove("/SOCD_Data.txt");
  File socd_file = LittleFS.open("/SOCD_Data.txt", "w");
  socd_file.println(SOCDToSave);
  socd_file.close();
}

void chooseSOCD() {
  // left held down - SOCD-N: left + right = neutral; up + down = neutral
  if (!leftRead and rightRead and upRead and downRead) {
    SOCD_mode = 0;
    saveSOCD(0);
  }

  // down held down - SOCD-U: left + right = neutral; down + up = up
  else if (leftRead and rightRead and upRead and !downRead) {
    SOCD_mode = 1;
    saveSOCD(1);
  }

  // right held down - SOCD-L: left + right = last command wins; up + down = last command wins
  else if (leftRead and !rightRead and upRead and downRead) {
    SOCD_mode = 2;
    saveSOCD(2);
  }

  // up held down - SOCD-LU: left + right = last command wins; up + down = up
  else if (leftRead and rightRead and !upRead and downRead) {
    SOCD_mode = 3;
    saveSOCD(3);
  }

  // up and down held down - SOCD-R: raw output, but on dualsense: left + right = neutral; up + down = neutral
  else if (leftRead and rightRead and !upRead and !downRead) {
    SOCD_mode = 4;
    saveSOCD(4);
  }

  Serial.print("SOCD_mode: ");
  Serial.println(SOCD_mode);
  display.fillRect(84, 0, 44, 16, 0);
  drawSOCDLabels();
}

void drawSOCDLabels() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (SOCD_mode == 0) {
    display.setCursor(98, 0);
    display.print("L+R:N");
    display.setCursor(98, 9);
    display.print("U+D:N");
  } else if (SOCD_mode == 1) {
    display.setCursor(98, 0);
    display.print("L+R:N");
    display.setCursor(98, 9);
    display.print("U+D:U");
  } else if (SOCD_mode == 2) {
    display.setCursor(92, 0);
    display.print("L+R:LC");
    display.setCursor(92, 9);
    display.print("U+D:LC");
  } else if (SOCD_mode == 3) {
    display.setCursor(92, 0);
    display.print("L+R:LC");
    display.setCursor(92, 9);
    display.print("U+D:U");
  } else if (SOCD_mode == 4) {
    display.setCursor(86, 0);
    display.print("L+R:RAW");
    display.setCursor(86, 9);
    display.print("U+D:RAW");
  }
}

void drawButtonLabels(uint8_t labelToRender) {
  if (labelToRender == 4 || labelToRender == 0) {  // left
    display.drawLine(53, 50, 53, 60, 1);
    display.drawLine(53, 50, 49, 54, 1);
    display.drawLine(53, 50, 57, 54, 1);
  }
  if (labelToRender == 5 || labelToRender == 0) {  // right
    display.drawLine(31, 21, 31, 29, 1);
    display.drawLine(28, 26, 31, 29, 1);
    display.drawLine(34, 26, 31, 29, 1);
  }
  if (labelToRender == 6 || labelToRender == 0) {  // up
    display.drawLine(11, 25, 19, 25, 1);
    display.drawLine(11, 25, 14, 22, 1);
    display.drawLine(11, 25, 14, 28, 1);
  }
  if (labelToRender == 7 || labelToRender == 0) {  // down
    display.drawLine(48, 26, 51, 29, 1);
    display.drawLine(43, 29, 51, 29, 1);
    display.drawLine(48, 32, 51, 29, 1);
  }
  if (labelToRender == 8 || labelToRender == 0) {  // square
    display.drawRect(62, 22, 7, 7, 1);
  }
  if (labelToRender == 9 || labelToRender == 0) {  // triangle
    display.drawTriangle(81, 19, 77, 26, 85, 26, 1);
  }
  if (labelToRender == 10 || labelToRender == 0) {  // r1
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(93, 27);
    u8g2_for_adafruit_gfx.print("R1");
  }
  if (labelToRender == 11 || labelToRender == 0) {  // l1
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(109, 28);
    u8g2_for_adafruit_gfx.print("L1");
  }
  if (labelToRender == 12 || labelToRender == 0) {  // x
    display.drawLine(60, 38, 66, 44, 1);
    display.drawLine(60, 44, 66, 38, 1);
  }
  if (labelToRender == 13 || labelToRender == 0) {  // circle
    display.drawCircle(79, 39, 4, 1);
  }
  if (labelToRender == 14 || labelToRender == 0) {  // r2
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(91, 43);
    u8g2_for_adafruit_gfx.print("R2");
  }
  if (labelToRender == 15 || labelToRender == 0) {  // l2
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(107, 44);
    u8g2_for_adafruit_gfx.print("L2");
  }
  if (labelToRender == 16 || labelToRender == 0) {  // home
    display.drawLine(1, 12, 1, 15, 1);
    display.drawLine(7, 12, 7, 15, 1);
    display.drawLine(1, 15, 3, 15, 1);
    display.drawLine(7, 15, 5, 15, 1);
    display.drawLine(3, 15, 3, 13, 1);
    display.drawLine(5, 15, 5, 13, 1);
    display.drawLine(5, 13, 3, 13, 1);
    display.drawLine(2, 12, 0, 12, 1);
    display.drawLine(6, 12, 8, 12, 1);
    display.drawLine(0, 12, 4, 8, 1);
    display.drawLine(8, 12, 4, 8, 1);
  }
  if (labelToRender == 17 || labelToRender == 0) {  // share
    display.drawCircle(20, 10, 1, 1);
    display.drawCircle(16, 12, 1, 1);
    display.drawCircle(20, 14, 1, 1);
    display.drawLine(19, 10, 17, 12, 1);
    display.drawLine(17, 12, 19, 14, 1);
  }
  if (labelToRender == 18 || labelToRender == 0) {  // options
    display.drawLine(29, 10, 35, 10, 1);
    display.drawLine(29, 12, 35, 12, 1);
    display.drawLine(29, 14, 35, 14, 1);
  }
  if (labelToRender == 19 || labelToRender == 0) {  // r3
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(42, 16);
    u8g2_for_adafruit_gfx.print("R3");
  }
  if (labelToRender == 20 || labelToRender == 0) {  // l3
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(56, 16);
    u8g2_for_adafruit_gfx.print("L3");
  }
  if (labelToRender == 21 || labelToRender == 0) {  // touch
    const uint8_t iconX = 71;
    const uint8_t iconY = 8;
    const int touchIcon[8][7] = {
        {0, 1, 1, 1, 1, 1, 0},
        {1, 1, 0, 0, 0, 1, 1},
        {1, 0, 1, 1, 1, 0, 1},
        {0, 1, 0, 0, 0, 1, 0},
        {1, 1, 0, 1, 0, 1, 1},
        {1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0, 1, 0},
    };

    for (int i = 0; i < 8; i++) {
      for (int ii = 0; ii < 7; ii++) {
        display.drawPixel(iconX + ii, iconY + i, touchIcon[i][ii]);
      }
    }
  }
}

void drawArt(uint8_t artX, uint8_t artY, const std::vector<std::vector<int> >& artToDraw) {
  for (int i = 0; i < artToDraw.size(); i++) {
    for (int j = 0; j < artToDraw[i].size(); j++) {
      display.drawPixel(artX + j, artY + i, artToDraw[i][j]);
    }
  }
}

void drawSpeechBubble(uint8_t color, String leftOrRight) {
  if (leftOrRight == "left") {
    display.drawLine(6, 35, 38, 35, color);
    display.drawLine(5, 36, 5, 44, color);
    display.drawLine(39, 36, 39, 44, color);
    display.drawLine(6, 45, 38, 45, color);
    display.drawTriangle(15, 45, 14, 52, 25, 45, color);
    display.drawLine(16, 45, 24, 45, 0);
    display.drawPixel(15, 52, color);
  } else if (leftOrRight == "right") {
    display.drawLine(6, 35, 38, 35, color);
    display.drawLine(5, 36, 5, 44, color);
    display.drawLine(39, 36, 39, 44, color);
    display.drawLine(6, 45, 38, 45, color);
    display.drawTriangle(15, 45, 30, 52, 25, 45, color);
    display.drawLine(17, 45, 25, 45, 0);
    display.drawPixel(29, 52, 0);
  } else if (leftOrRight == "both") {
    display.drawLine(6, 35, 38, 35, color);
    display.drawLine(5, 36, 5, 44, color);
    display.drawLine(39, 36, 39, 44, color);
    display.drawLine(6, 45, 38, 45, color);
    display.drawTriangle(15, 45, 14, 52, 25, 45, color);
    display.drawPixel(15, 52, color);
    display.drawTriangle(15, 45, 30, 52, 25, 45, color);
    display.drawPixel(29, 52, 0);
    display.drawLine(16, 45, 26, 45, 0);
    display.drawLine(15, 45, 20, 47, 0);
    display.drawLine(25, 45, 20, 48, 0);
    display.drawPixel(20, 48, color);
    display.drawPixel(20, 47, color);
    display.drawPixel(21, 47, color);
  }
}

void drawSpeechText(bool clear) {
  if (clear == 1) {
    drawSpeechBubble(0, "both");
    display.fillRect(6, 37, 42, 7, 0);
  } else {
    if (SOCD_speech_text == "L+R:R" || SOCD_speech_text == "L+R:L" || SOCD_speech_text == "L+R:N") {
      drawSpeechBubble(1, "left");
    } else if (SOCD_speech_text == "U+D:U" || SOCD_speech_text == "U+D:D" || SOCD_speech_text == "U+D:N") {
      drawSpeechBubble(1, "right");
    } else {
      drawSpeechBubble(1, "both");
    }

    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_profont11_tf);
    u8g2_for_adafruit_gfx.setCursor(8, 44);
    u8g2_for_adafruit_gfx.print(SOCD_speech_text);
  }
}

void drawButtonCircles(uint8_t circleToRender) {
  if (circleToRender == 4 || circleToRender == 0)
    display.drawCircle(53, 55, 8, 1);  // up
  if (circleToRender == 5 || circleToRender == 0)
    display.drawCircle(31, 25, 7, 1);  // down
  if (circleToRender == 6 || circleToRender == 0)
    display.drawCircle(15, 25, 7, 1);  // left
  if (circleToRender == 7 || circleToRender == 0)
    display.drawCircle(47, 29, 7, 1);  // right
  if (circleToRender == 8 || circleToRender == 0)
    display.drawCircle(65, 25, 7, 1);  // square
  if (circleToRender == 9 || circleToRender == 0)
    display.drawCircle(81, 23, 7, 1);  // triangle
  if (circleToRender == 10 || circleToRender == 0)
    display.drawCircle(97, 23, 7, 1);  // r1
  if (circleToRender == 11 || circleToRender == 0)
    display.drawCircle(113, 24, 7, 1);  // l1
  if (circleToRender == 12 || circleToRender == 0)
    display.drawCircle(63, 41, 7, 1);  // x
  if (circleToRender == 13 || circleToRender == 0)
    display.drawCircle(79, 39, 7, 1);  // circle
  if (circleToRender == 14 || circleToRender == 0)
    display.drawCircle(95, 39, 7, 1);  // r2
  if (circleToRender == 15 || circleToRender == 0)
    display.drawCircle(111, 40, 7, 1);  // l2
  if (circleToRender == 16 || circleToRender == 0)
    display.drawCircle(4, 3, 3, 1);  // home
  if (circleToRender == 17 || circleToRender == 0)
    display.drawCircle(18, 3, 3, 1);  // share
  if (circleToRender == 18 || circleToRender == 0)
    display.drawCircle(32, 3, 3, 1);  // options
  if (circleToRender == 19 || circleToRender == 0)
    display.drawCircle(46, 3, 3, 1);  // r3
  if (circleToRender == 20 || circleToRender == 0)
    display.drawCircle(60, 3, 3, 1);  // l3
  if (circleToRender == 21 || circleToRender == 0)
    display.drawCircle(74, 3, 3, 1);  // touch
}

void controlButtonVisualRenderHelper(uint8_t circleToFill, bool color) {
  if (circleToFill == 4)
    display.fillCircle(53, 55, 8, color);  // up
  if (circleToFill == 5)
    display.fillCircle(31, 25, 7, color);  // down
  if (circleToFill == 6)
    display.fillCircle(15, 25, 7, color);  // left
  if (circleToFill == 7)
    display.fillCircle(47, 29, 7, color);  // right
  if (circleToFill == 8)
    display.fillCircle(65, 25, 7, color);  // square
  if (circleToFill == 9)
    display.fillCircle(81, 23, 7, color);  // triangle
  if (circleToFill == 10)
    display.fillCircle(97, 23, 7, color);  // r1
  if (circleToFill == 11)
    display.fillCircle(113, 24, 7, color);  // l1
  if (circleToFill == 12)
    display.fillCircle(63, 41, 7, color);  // x
  if (circleToFill == 13)
    display.fillCircle(79, 39, 7, color);  // circle
  if (circleToFill == 14)
    display.fillCircle(95, 39, 7, color);  // r2
  if (circleToFill == 15)
    display.fillCircle(111, 40, 7, color);  // l2
  if (circleToFill == 16)
    display.fillCircle(4, 3, 3, color);  // home
  if (circleToFill == 17)
    display.fillCircle(18, 3, 3, color);  // share
  if (circleToFill == 18)
    display.fillCircle(32, 3, 3, color);  // options
  if (circleToFill == 19)
    display.fillCircle(46, 3, 3, color);  // r3
  if (circleToFill == 20)
    display.fillCircle(60, 3, 3, color);  // l3
  if (circleToFill == 21)
    display.fillCircle(74, 3, 3, color);  // touch
}

void controlButtonVisualRender(uint8_t direction, bool& flag, bool& flag2) {
  uint8_t readButton = digitalRead(direction);
  if (direction == 4 || direction == 5 || direction == 6 || direction == 7) {
    readButton = !gpio_get_dir(direction);
  } else if (direction == 19 || direction == 20) {
    readButton = !readButton;  // R3 & L3 are opposite to the rest
  }

  if (!readButton && flag) {
    flag = false;
    flag2 = true;
    controlButtonVisualRenderHelper(direction, !flag);
  } else if (readButton && flag2) {
    flag = true;
    flag2 = false;
    controlButtonVisualRenderHelper(direction, !flag);
    drawButtonCircles(direction);
    drawButtonLabels(direction);
  }
}
