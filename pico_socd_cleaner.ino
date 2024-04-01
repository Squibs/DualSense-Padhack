#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "squibs_logo.h"
#include "LittleFS.h" // LittleFS is declared

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32; 0x3C for my particular display which is 12x64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET, 400000); // Wire1 for GP 26/27 instead of Wire0 for GP 4/5
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
  Serial.begin(9600); // Allows printlns / monitoring

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  u8g2_for_adafruit_gfx.begin(display);

  // Display logo
  display.clearDisplay();
  display.drawBitmap(0, 5, epd_bitmap_Squibs, 128, 64, WHITE);
  display.display();
  delay(2000); // Pause for 2 seconds

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

  // all other buttons
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
  pinMode(btnR3, INPUT_PULLDOWN); // R3 & L3 are active Highs, opposite of the rest
  pinMode(btnL3, INPUT_PULLDOWN); // R3 & L3 are active Highs, opposite of the rest
  pinMode(btnTouch, INPUT_PULLUP);
  pinMode(btnChooseSOCD, INPUT_PULLUP);
  
  // check if a saved socd mode exists, if not create one
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
  
  display.clearDisplay();
  drawHitBoxCircles("all");
  drawHitBoxLabels("all");
  drawSOCDLabel();
  display.setTextSize(1);
  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_ImpactBits_tr);
  u8g2_for_adafruit_gfx.setCursor(85, 63);
  u8g2_for_adafruit_gfx.print("SQUIBS");
  display.display();
}

void loop() {
  currentMillis = millis();
  leftRead = digitalRead(leftIN);
  rightRead = digitalRead(rightIN);
  upRead = digitalRead(upIN);
  downRead = digitalRead(downIN);

  if (!digitalRead(btnChooseSOCD) && SOCDChangeFlag) {
    chooseSOCD();
    SOCDChangeFlag = false;
    SOCDChangeFlag2 = true;
  } else if (digitalRead(btnChooseSOCD) && SOCDChangeFlag2) {
    SOCDChangeFlag = true;
    SOCDChangeFlag2 = false;
  }

  if (SOCD_mode == 0) { // left held down - SOCD-N: left + right = neutral; up + down = neutral
    useSOCD0();
  } else if (SOCD_mode == 1) { // down held down - SOCD-U: left + right = neutral; down + up = up
    useSOCD1();
  } else if (SOCD_mode == 2) { // right held down - SOCD-L: left + right = last command wins; up + down = last command wins
    useSOCD2();
  } else if (SOCD_mode == 3) { // up held down - SOCD-LU: left + right = last command wins; up + down = up
    useSOCD3();
  } else if (SOCD_mode == 4) { // up and down held down - SOCD-R: raw output, but on dualsense: left + right = neutral; up + down = neutral
    useSOCD4();
  }

  if (SOCD_last_value != SOCD_speech_text) {
    SOCD_last_value = SOCD_speech_text;

    if (SOCD_speech_text == " ") {
      drawSpeechText(1);
    } else {
      drawSpeechText(1);
      drawSpeechText(0);
    }
  }

  // directional buttons
  if (gpio_get_dir(4) && upFlag) {
    display.fillCircle(53, 55, 8, 1);
    upFlag = false;
    upFlag2 = true;
  } else if (!gpio_get_dir(4) && upFlag2) {
    display.fillCircle(53, 55, 8, 0);
    drawHitBoxCircles("up");
    drawHitBoxLabels("up");
    upFlag = true;
    upFlag2 = false;
  }

  if (gpio_get_dir(5) && downFlag) {
    display.fillCircle(31, 25, 7, 1);
    downFlag = false;
    downFlag2 = true;
  } else if (!gpio_get_dir(5) && downFlag2) {
    display.fillCircle(31, 25, 7, 0);
    drawHitBoxCircles("down");
    drawHitBoxLabels("down");
    downFlag = true;
    downFlag2 = false;
  }

  if (gpio_get_dir(6) && leftFlag) {
    display.fillCircle(15, 25, 7, 1);
    leftFlag = false;
    leftFlag2 = true;
  } else if (!gpio_get_dir(6) && leftFlag2) {
    display.fillCircle(15, 25, 7, 0);
    drawHitBoxCircles("left");
    drawHitBoxLabels("left");
    leftFlag = true;
    leftFlag2 = false;
  }

  if (gpio_get_dir(7) && rightFlag) {
    display.fillCircle(47, 29, 7, 1);
    rightFlag = false;
    rightFlag2 = true;
  } else if (!gpio_get_dir(7) && rightFlag2) {
    display.fillCircle(47, 29, 7, 0);
    drawHitBoxCircles("right");
    drawHitBoxLabels("right");
    rightFlag = true;
    rightFlag2 = false;
  }

  // all other buttons
  uint8_t squareRead = digitalRead(btnSquare);
  if (!squareRead && squareFlag) {
    display.fillCircle(65, 25, 7, 1);
    squareFlag = false;
    squareFlag2 = true;
  } else if (squareRead && squareFlag2) {
    display.fillCircle(65, 25, 7, 0);
    drawHitBoxCircles("square");
    drawHitBoxLabels("square");
    squareFlag = true;
    squareFlag2 = false;
  }

  uint8_t triangleRead = digitalRead(btnTriangle);
  if (!triangleRead && triangleFlag) {
    display.fillCircle(81, 23, 7, 1);
    triangleFlag = false;
    triangleFlag2 = true;
  } else if (triangleRead && triangleFlag2) {
    display.fillCircle(81, 23, 7, 0);
    drawHitBoxCircles("triangle");
    drawHitBoxLabels("triangle");
    triangleFlag = true;
    triangleFlag2 = false;
  }

  uint8_t xRead = digitalRead(btnX);
  if (!xRead && xFlag) {
    display.fillCircle(63, 41, 7, 1);
    xFlag = false;
    xFlag2 = true;
  } else if (xRead && xFlag2) {
    display.fillCircle(63, 41, 7, 0);
    drawHitBoxCircles("x");
    drawHitBoxLabels("x");
    xFlag = true;
    xFlag2 = false;
  }

  uint8_t circleRead = digitalRead(btnCircle);
  if (!circleRead && circleFlag) {
    display.fillCircle(79, 39, 7, 1);
    circleFlag = false;
    circleFlag2 = true;
  } else if (circleRead && circleFlag2) {
    display.fillCircle(79, 39, 7, 0);
    drawHitBoxCircles("circle");
    drawHitBoxLabels("circle");
    circleFlag = true;
    circleFlag2 = false;
  }

  uint8_t r1Read = digitalRead(btnR1);
  if (!r1Read && r1Flag) {
    display.fillCircle(97, 23, 7, 1);
    r1Flag = false;
    r1Flag2 = true;
  } else if (r1Read && r1Flag2) {
    display.fillCircle(97, 23, 7, 0);
    drawHitBoxCircles("r1");
    drawHitBoxLabels("r1");
    r1Flag = true;
    r1Flag2 = false;
  }

  uint8_t l1Read = digitalRead(btnL1);
  if (!l1Read && l1Flag) {
    display.fillCircle(113, 24, 7, 1);
    l1Flag = false;
    l1Flag2 = true;
  } else if (l1Read && l1Flag2) {
    display.fillCircle(113, 24, 7, 0);
    drawHitBoxCircles("l1");
    drawHitBoxLabels("l1");
    l1Flag = true;
    l1Flag2 = false;
  }

  uint8_t r2Read = digitalRead(btnR2);
  if (!r2Read && r2Flag) {
    display.fillCircle(95, 39, 7, 1);
    r2Flag = false;
    r2Flag2 = true;
  } else if (r2Read && r2Flag2) {
    display.fillCircle(95, 39, 7, 0);
    drawHitBoxCircles("r2");
    drawHitBoxLabels("r2");
    r2Flag = true;
    r2Flag2 = false;
  }

  uint8_t l2Read = digitalRead(btnL2);
  if (!l2Read && l2Flag) {
    display.fillCircle(111, 40, 7, 1);
    l2Flag = false;
    l2Flag2 = true;
  } else if (l2Read && l2Flag2) {
    display.fillCircle(111, 40, 7, 0);
    drawHitBoxCircles("l2");
    drawHitBoxLabels("l2");
    l2Flag = true;
    l2Flag2 = false;
  }

  uint8_t homeRead = digitalRead(btnHome);
  if (!homeRead && homeFlag) {
    display.fillCircle(4, 3, 3, 1);
    homeFlag = false;
    homeFlag2 = true;
  } else if (homeRead && homeFlag2) {
    display.fillCircle(4, 3, 3, 0);
    drawHitBoxCircles("home");
    drawHitBoxLabels("home");
    homeFlag = true;
    homeFlag2 = false;
  }

  uint8_t shareRead = digitalRead(btnShare);
  if (!shareRead && shareFlag) {
    display.fillCircle(18, 3, 3, 1);
    shareFlag = false;
    shareFlag2 = true;
  } else if (shareRead && shareFlag2) {
    display.fillCircle(18, 3, 3, 0);
    drawHitBoxCircles("share");
    drawHitBoxLabels("share");
    shareFlag = true;
    shareFlag2 = false;
  }

  uint8_t optionsRead = digitalRead(btnOptions);
  if (!optionsRead && optionsFlag) {
    display.fillCircle(32, 3, 3, 1);
    optionsFlag = false;
    optionsFlag2 = true;
  } else if (optionsRead && optionsFlag2) {
    display.fillCircle(32, 3, 3, 0);
    drawHitBoxCircles("options");
    drawHitBoxLabels("options");
    optionsFlag = true;
    optionsFlag2 = false;
  }

  uint8_t touchRead = digitalRead(btnTouch);
  if (!touchRead && touchFlag) {
    display.fillCircle(74, 3, 3, 1);
    touchFlag = false;
    touchFlag2 = true;
  } else if (touchRead && touchFlag2) {
    display.fillCircle(74, 3, 3, 0);
    drawHitBoxCircles("touch");
    drawHitBoxLabels("touch");
    touchFlag = true;
    touchFlag2 = false;
  }

  // l3 - R3 & L3 are active Highs, opposite of the rest
  uint8_t r3Read = digitalRead(btnR3);
  if (r3Read && r3Flag) {
    display.fillCircle(46, 3, 3, 1);
    r3Flag = false;
    r3Flag2 = true;
  } else if (!r3Read && r3Flag2) {
    display.fillCircle(46, 3, 3, 0);
    drawHitBoxCircles("r3");
    drawHitBoxLabels("r3");
    r3Flag = true;
    r3Flag2 = false;
  }

  // # l3 - R3 & L3 are active Highs, opposite of the rest
  uint8_t l3Read = digitalRead(btnL3);
  if (l3Read && l3Flag) {
    display.fillCircle(60, 3, 3, 1);
    l3Flag = false;
    l3Flag2 = true;
  } else if (!l3Read && l3Flag2) {
    display.fillCircle(60, 3, 3, 0);
    drawHitBoxCircles("l3");
    drawHitBoxLabels("l3");
    l3Flag = true;
    l3Flag2 = false;
  }

  // art animation control
  art_animation_control -= 1;
  if (art_animation_control <= 0) {
    art_animation_control = art_animation_speed;
    art_animation_flag = true;
  } else if (art_animation_control == round(art_animation_speed / 2)) {
    art_animation_flag = true;
  }

  if (art_animation_control > round(art_animation_speed / 2) && art_animation_flag) {
    drawPirate("1");
    drawSquid("1");
    drawCat("1");
    drawSagat("1");
  } else if (art_animation_control < round(art_animation_speed / 2) && art_animation_flag) {
    drawPirate("2");
    drawSquid("2");
    drawCat("2");
    drawSagat("2");
  }

  display.display();
}

void useSOCD0() { // left held down - SOCD-N: left + right = neutral; up + down = neutral
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

void useSOCD1() { // down held down - SOCD-U: left + right = neutral; down + up = up
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

void useSOCD2() { // right held down - SOCD-L: left + right = last command wins; up + down = last command wins
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
      pinMode(rightOUT,OUTPUT);
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

void useSOCD3() { // up held down - SOCD-LU: left + right = last command wins; up + down = up
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

void useSOCD4() { // up and down held down - SOCD-R: raw output, but on dualsense: left + right = neutral; up + down = neutral
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
  drawSOCDLabel();
}

void drawSOCDLabel() {
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

void drawHitBoxCircles(String circleToRender) {
  if (circleToRender == "left" || circleToRender == "all") {
    display.drawCircle(15, 25, 7, 1);
  }
  if (circleToRender == "down" || circleToRender == "all") {
    display.drawCircle(31, 25, 7, 1);
  }
  if (circleToRender == "right" || circleToRender == "all") {
    display.drawCircle(47, 29, 7, 1);
  }
  if (circleToRender == "up" || circleToRender == "all") {
    display.drawCircle(53, 55, 8, 1);
  }
  if (circleToRender == "square" || circleToRender == "all") {
    display.drawCircle(65, 25, 7, 1);
  }
  if (circleToRender == "x" || circleToRender == "all") {
    display.drawCircle(63, 41, 7, 1);
  }
  if (circleToRender == "triangle" || circleToRender == "all") {
    display.drawCircle(81, 23, 7, 1);
  }
  if (circleToRender == "circle" || circleToRender == "all") {
    display.drawCircle(79, 39, 7, 1);
  }
  if (circleToRender == "r1" || circleToRender == "all") {
    display.drawCircle(97, 23, 7, 1);
  }
  if (circleToRender == "r2" || circleToRender == "all") {
    display.drawCircle(95, 39, 7, 1);
  }
  if (circleToRender == "l1" || circleToRender == "all") {
    display.drawCircle(113, 24, 7, 1);
  }
  if (circleToRender == "l2" || circleToRender == "all") {
    display.drawCircle(111, 40, 7, 1);
  }
  if (circleToRender == "home" || circleToRender == "all") {
    display.drawCircle(4, 3, 3, 1);
  }
  if (circleToRender == "share" || circleToRender == "all") {
    display.drawCircle(18, 3, 3, 1); 
  }
  if (circleToRender == "options" || circleToRender == "all") {
    display.drawCircle(32, 3, 3, 1);
  }
  if (circleToRender == "r3" || circleToRender == "all") {
    display.drawCircle(46, 3, 3, 1);
  }
  if (circleToRender == "l3" || circleToRender == "all") {
    display.drawCircle(60, 3, 3, 1);
  }
  if (circleToRender == "touch" || circleToRender == "all") {
    display.drawCircle(74, 3, 3, 1);
  }
}

void drawHitBoxLabels(String labelToRender) {
  if (labelToRender == "circle" || labelToRender == "all") {
    display.drawCircle(79, 39, 4, 1);
  }
  if (labelToRender == "triangle" || labelToRender == "all") {
    display.drawTriangle(81, 19, 77, 26, 85, 26, 1);
  }
  if (labelToRender == "square" || labelToRender == "all") {
    display.drawRect(62, 22, 7, 7, 1);
  }
  if (labelToRender == "x" || labelToRender == "all") {
    display.drawLine(60, 38, 66, 44, 1);
    display.drawLine(60, 44, 66, 38, 1);
  }
  if (labelToRender == "left" || labelToRender == "all") {
    display.drawLine(11, 25, 19, 25, 1);
    display.drawLine(11, 25, 14, 22, 1);
    display.drawLine(11, 25, 14, 28, 1);
  }
  if (labelToRender == "down" || labelToRender == "all") {
    display.drawLine(31, 21, 31, 29, 1);
    display.drawLine(28, 26, 31, 29, 1);
    display.drawLine(34, 26, 31, 29, 1);
  }
  if (labelToRender == "right" || labelToRender == "all") {
    display.drawLine(48, 26, 51, 29, 1);
    display.drawLine(43, 29, 51, 29, 1);
    display.drawLine(48, 32, 51, 29, 1);
  }
  if (labelToRender == "up" || labelToRender == "all") {
    display.drawLine(53, 50, 53, 60, 1);
    display.drawLine(53, 50, 49, 54, 1);
    display.drawLine(53, 50, 57, 54, 1);
  }
  if (labelToRender == "r1" || labelToRender == "all") {
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(93, 27);
    u8g2_for_adafruit_gfx.print("R1");
  }
  if (labelToRender == "r2" || labelToRender == "all") {
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(91, 43);
    u8g2_for_adafruit_gfx.print("R2");
  }
  if (labelToRender == "l1" || labelToRender == "all") {
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(109, 28);
    u8g2_for_adafruit_gfx.print("L1");
  }
  if (labelToRender == "l2" || labelToRender == "all") {
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
    u8g2_for_adafruit_gfx.setCursor(107, 44);
    u8g2_for_adafruit_gfx.print("L2");
  }

  // home
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

  // share
  display.drawCircle(20, 10, 1, 1);
  display.drawCircle(16, 12, 1, 1);
  display.drawCircle(20, 14, 1, 1);
  display.drawLine(19, 10, 17, 12, 1);
  display.drawLine(17, 12, 19, 14, 1);

  // options
  display.drawLine(29, 10, 35, 10, 1);
  display.drawLine(29, 12, 35, 12, 1);
  display.drawLine(29, 14, 35, 14, 1);

  // r3
  u8g2_for_adafruit_gfx.setFont(u8g2_font_5x7_tf);
  u8g2_for_adafruit_gfx.setCursor(42, 16);
  u8g2_for_adafruit_gfx.print("R3");

  // l3
  u8g2_for_adafruit_gfx.setCursor(56, 16);
  u8g2_for_adafruit_gfx.print("L");
  u8g2_for_adafruit_gfx.setCursor(61, 16);
  u8g2_for_adafruit_gfx.print("3");

  // touch
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

void drawPirate(String frame) {
  const uint8_t pirateX = 0;
  const uint8_t pirateY = 49;

  const int pirate1[15][13] =  {
    {0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0},
    {0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0},
  };

  const int pirate2[15][13] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0},
  };

  if (frame == "1") {
    for (int i = 0; i < 15; i++) {
      for (int ii = 0; ii < 13; ii++) {
        display.drawPixel(pirateX + ii, pirateY + i, pirate1[i][ii]);
      }
    }
  } else if (frame == "2") {
    for (int i = 0; i < 15; i++) {
      for (int ii = 0; ii < 13; ii++) {
        display.drawPixel(pirateX + ii, pirateY + i, pirate2[i][ii]);
      }
    }
  }
}

void drawSquid(String frame) {
  const uint8_t squidX = 66;
  const uint8_t squidY = 49;

  const int squid1[14][16] =  {
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,1,1,1,0,1,1,1,1,1,1,0,0,0},
    {0,0,1,1,0,1,1,1,0,1,1,1,1,1,0,0},
    {0,0,1,0,1,0,1,1,1,1,1,1,1,1,0,0},
    {0,0,1,1,0,1,1,1,1,1,1,1,1,0,1,0},
    {0,0,1,1,1,1,1,1,1,1,0,1,1,0,1,0},
    {1,0,0,1,1,1,1,0,1,1,0,1,1,1,1,0},
    {0,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0},
    {1,0,0,1,1,1,0,0,1,1,0,1,1,1,1,1},
    {0,1,1,1,0,0,1,1,0,0,1,1,1,0,1,1},
    {0,0,0,0,1,1,1,1,1,1,1,1,1,0,1,1},
    {0,0,1,1,1,0,0,0,1,1,1,1,0,1,1,0},
  };

  const int squid2[14][16] = {
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,1,1,1,0,1,1,1,1,1,1,0,0,0},
    {0,0,1,1,0,1,1,1,0,1,1,1,1,1,0,0},
    {0,0,1,0,1,0,1,1,1,1,1,1,1,1,0,0},
    {0,0,1,1,0,1,1,1,1,1,1,1,1,0,1,0},
    {0,0,1,1,1,1,1,1,1,1,0,1,1,0,1,0},
    {0,0,0,1,1,1,1,0,1,1,0,1,1,1,1,0},
    {0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0},
    {1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1},
    {0,1,0,0,0,1,1,1,1,1,1,1,1,0,1,1},
    {0,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0},
  }; 

  if (frame == "1") {
    for (int i = 0; i < 14; i++) {
      for (int ii = 0; ii < 16; ii++) {
        display.drawPixel(squidX + ii, squidY + i, squid1[i][ii]);
      }
    }
  } else if (frame == "2") {
    for (int i = 0; i < 14; i++) {
      for (int ii = 0; ii < 16; ii++) {
        display.drawPixel(squidX + ii, squidY + i, squid2[i][ii]);
      }
    }
  }
}

void drawSagat(String frame) {
  const uint8_t sagatX = 30;
  const uint8_t sagatY = 48;

  const int sagat1[16][12] = {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,1,0,0,1,1,1,0,1,0,0},
    {0,0,0,1,0,0,0,0,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,0,0},
    {0,0,0,1,1,1,1,1,0,0,0,0},
    {0,0,0,1,1,1,1,0,0,1,1,0},
    {0,1,0,0,0,0,0,0,1,1,1,1},
    {1,0,0,1,0,1,1,1,0,1,1,1},
    {1,0,0,0,1,0,1,0,0,0,1,1},
    {1,1,1,0,0,0,0,1,1,0,1,1},
    {0,1,1,0,1,1,0,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,0,1,1,1,0,0,0},
    {0,0,1,1,1,0,0,0,1,1,1,0},
  };

  const int sagat2[16][12] = {
    {0,0,0,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,1,0,0,1,1,1,0,1,0,0},
    {0,0,0,1,0,0,0,0,1,1,0,0},
    {0,0,1,1,0,0,0,1,0,0,0,0},
    {0,0,0,1,1,1,1,1,0,0,0,0},
    {0,0,0,1,1,1,1,0,0,1,1,0},
    {0,1,0,0,0,0,0,0,1,1,1,1},
    {1,0,0,1,0,1,1,1,0,1,1,1},
    {1,0,0,0,1,0,1,0,0,0,1,1},
    {1,1,1,0,0,0,0,1,1,0,1,1},
    {0,1,1,0,1,1,0,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,0,1,1,1,0,0,0},
    {0,0,0,1,1,0,0,1,1,1,0,0},
    {0,0,1,1,1,0,0,0,1,1,1,0},
  };

  if (frame == "1") {
    for (int i = 0; i < 16; i++) {
      for (int ii = 0; ii < 12; ii++) {
        display.drawPixel(sagatX + ii, sagatY + i, sagat1[i][ii]);
      }
    }
  } else if (frame == "2") {
    for (int i = 0; i < 16; i++) {
      for (int ii = 0; ii < 12; ii++) {
        display.drawPixel(sagatX + ii, sagatY + i, sagat2[i][ii]);
      }
    }
  }
}

void drawCat(String frame) {
  const uint8_t catX = 17;
  const uint8_t catY = 55;

  const int cat1[9][10] =  {
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 1, 1, 0, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 0, 1, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  };

  const int cat2[9][10] = {
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 1, 1, 0, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 0, 1, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  }; 

  if (frame == "1") {
    for (int i = 0; i < 9; i++) {
      for (int ii = 0; ii < 10; ii++) {
        display.drawPixel(catX + ii, catY + i, cat1[i][ii]);
      }
    }
  } else if (frame == "2") {
    for (int i = 0; i < 9; i++) {
      for (int ii = 0; ii < 10; ii++) {
        display.drawPixel(catX + ii, catY + i, cat2[i][ii]);
      }
    }
  }
}

void drawBubble(uint8_t color, String leftOrRight) {
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
    drawBubble(0, "both");
    display.fillRect(6, 37, 42, 7, 0);
  } else {
    if (SOCD_speech_text == "L+R:R" || SOCD_speech_text == "L+R:L" || SOCD_speech_text == "L+R:N") {
      drawBubble(1, "left");
    } else if (SOCD_speech_text == "U+D:U" || SOCD_speech_text == "U+D:D" || SOCD_speech_text == "U+D:N") {
      drawBubble(1, "right");
    } else {
      drawBubble(1, "both");
    }
    
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
    // u8g2_for_adafruit_gfx.setFont(u8g2_font_profont10_tf);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_profont11_tf);
    u8g2_for_adafruit_gfx.setCursor(8, 44);
    u8g2_for_adafruit_gfx.print(SOCD_speech_text);
  }
}
