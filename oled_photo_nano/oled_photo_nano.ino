#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <math.h>

// ======================================================
// OLED
// ======================================================

U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0);

// ======================================================
// PINS
// ======================================================

#define LDR_PIN A0


// ======================================================
// MOODS
// ======================================================

enum Mood {
  SLEEP,
  NORMAL,
  ANNOYED
};

Mood mood = NORMAL;

// ======================================================
// ANIMATION
// ======================================================

bool eyesClosed = false;

unsigned long lastBlink = 0;

unsigned long lastHeart = 0;
bool showHeart = false;

int breatheOffset = 0;

// ======================================================
// LIGHT
// ======================================================

int lightValue = 0;

// VALORES PARA NANO (0-1023)
const int DARK_THRESHOLD = 100;
const int BRIGHT_THRESHOLD = 700;

// ======================================================
// SETUP
// ======================================================

void setup() {

  Wire.begin();

  display.begin();

  randomSeed(analogRead(LDR_PIN));

  bootScreen();
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  readLight();

  updateMood();

  updateBlink();

  updateHeart();

  render();

  delay(30);
}

// ======================================================
// BOOT SCREEN
// ======================================================

void bootScreen() {

  display.firstPage();

  do {

    display.setFont(u8g2_font_helvB10_tr);

    display.drawStr(18, 28, "by Dai!");

    display.setFont(u8g2_font_helvR10_tr);

    display.drawStr(48, 48, "2026 :D");

  } while(display.nextPage());

  delay(2200);

  // mini blink cute
  for(int i = 0; i < 2; i++) {

    display.firstPage();

    do {

      display.drawLine(44, 34, 52, 34);
      display.drawLine(76, 34, 84, 34);

    } while(display.nextPage());

    delay(120);

    display.firstPage();

    do {

      display.drawDisc(45, 34, 5);
      display.drawDisc(83, 34, 5);

    } while(display.nextPage());

    delay(180);
  }
}

// ======================================================
// LIGHT SENSOR
// ======================================================

void readLight() {

  lightValue = analogRead(LDR_PIN);
}

// ======================================================
// MOOD LOGIC
// ======================================================

void updateMood() {

  if(lightValue < DARK_THRESHOLD) {

    mood = SLEEP;
  }

  else if(lightValue > BRIGHT_THRESHOLD) {

    mood = ANNOYED;
  }

  else {

    mood = NORMAL;
  }
}

// ======================================================
// BLINK
// ======================================================

void updateBlink() {

  static int blinkStage = 0;

  unsigned long now = millis();

  int blinkRate;

  if(mood == ANNOYED)
    blinkRate = random(1800, 3500);

  else
    blinkRate = random(3000, 7000);

  // iniciar blink
  if(blinkStage == 0 && now - lastBlink > blinkRate) {

    eyesClosed = true;

    blinkStage = 1;

    lastBlink = now;
  }

  // abrir despues primer blink
  else if(blinkStage == 1 && now - lastBlink > 120) {

    eyesClosed = false;

    if(mood == ANNOYED) {

      blinkStage = 2;
    }

    else {

      blinkStage = 0;
    }

    lastBlink = now;
  }

  // segundo blink
  else if(blinkStage == 2 && now - lastBlink > 90) {

    eyesClosed = true;

    blinkStage = 3;

    lastBlink = now;
  }

  // terminar doble blink
  else if(blinkStage == 3 && now - lastBlink > 120) {

    eyesClosed = false;

    blinkStage = 0;

    lastBlink = now;
  }
}

// ======================================================
// HEART RANDOM
// ======================================================

void updateHeart() {

  unsigned long now = millis();

  if(mood == NORMAL) {

    if(!showHeart && random(0, 2500) > 2492) {

      showHeart = true;

      lastHeart = now;
    }

    if(showHeart && now - lastHeart > 1200) {

      showHeart = false;
    }
  }

  else {

    showHeart = false;
  }
}

// ======================================================
// RENDER
// ======================================================

void render() {

  breatheOffset = sin(millis() * 0.002) * 2;

  display.firstPage();

  do {

    switch(mood) {

      case SLEEP:
        drawSleepFace();
        break;

      case NORMAL:
        drawNormalFace();
        break;

      case ANNOYED:
        drawAnnoyedFace();
        break;
    }

  } while(display.nextPage());
}

// ======================================================
// NORMAL FACE
// ======================================================

void drawNormalFace() {

  int y = 30 + breatheOffset;

  if(eyesClosed) {

    display.drawLine(40, y, 50, y);
    display.drawLine(78, y, 88, y);
  }

  else {

    display.drawDisc(45, y, 5);
    display.drawDisc(83, y, 5);
  }

  // heart
  if(showHeart) {

    int heartX = 20 + sin(millis() * 0.004);
    int heartY = 14 + sin(millis() * 0.01) * 2;

    drawHeart(heartX, heartY);
  }
}

// ======================================================
// SLEEP FACE
// ======================================================

void drawSleepFace() {

  int y = 32;

  display.drawLine(40, y, 50, y);
  display.drawLine(78, y, 88, y);

  int zOffset = (millis() / 300) % 10;

  display.setFont(u8g2_font_6x12_tr);

  display.drawStr(58, 20 - zOffset, "z");
  display.drawStr(68, 14 - zOffset, "Z");
}

// ======================================================
// BRIGHT LIGHT FACE
// ======================================================

void drawAnnoyedFace() {

  int y = 30 + breatheOffset;

  if(eyesClosed) {

    // >
    display.drawLine(38, y - 4, 45, y);
    display.drawLine(38, y + 4, 45, y);

    // <
    display.drawLine(90, y - 4, 83, y);
    display.drawLine(90, y + 4, 83, y);
  }

  else {

    display.drawDisc(45, y, 5);
    display.drawDisc(83, y, 5);
  }
}

// ======================================================
// HEART
// ======================================================

void drawHeart(int x, int y) {

  display.drawDisc(x - 2, y, 3);
  display.drawDisc(x + 2, y, 3);

  display.drawTriangle(
    x - 5, y + 1,
    x + 5, y + 1,
    x, y + 8
  );
}
