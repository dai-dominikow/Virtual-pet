#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <math.h>

// =========================
// OLED
// =========================

// Si no funciona:
// cambiar SH1106 por SSD1306
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0);

// =========================
// PINS
// =========================

#define SDA_PIN 4
#define SCL_PIN 5

#define LDR_PIN 0

// =========================
// MOODS
// =========================

enum Mood {
  SLEEP,
  NORMAL,
  ANNOYED
};

Mood mood = NORMAL;

// =========================
// ANIMATION
// =========================

bool eyesClosed = false;

unsigned long lastBlink = 0;
unsigned long blinkDuration = 120;

unsigned long lastHeart = 0;
bool showHeart = false;

float breatheOffset = 0;

// =========================
// LIGHT
// =========================

int lightValue = 0;

// calibrar después
const int DARK_THRESHOLD = 300;
const int BRIGHT_THRESHOLD = 2000;

// =========================
// SETUP
// =========================

void setup() {

  Wire.begin(SDA_PIN, SCL_PIN);

  display.begin();

  randomSeed(analogRead(LDR_PIN));
  
  bootScreen();
}

// =========================
// LOOP
// =========================

void loop() {

  readLight();

  updateMood();

  updateBlink();

  updateHeart();

  render();

  delay(30);
}

// =========================
// LIGHT SENSOR
// =========================

void readLight() {

  lightValue = analogRead(LDR_PIN);
}

// =========================
// MOOD LOGIC
// =========================

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

// =========================
// BLINK
// =========================

void updateBlink() {

  static int blinkStage = 0;

  unsigned long now = millis();

  int blinkRate;

  // mucha luz = pestañea más seguido
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

    // doble blink solo con mucha luz
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

// =========================
// HEART RANDOM
// =========================

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

// =========================
// RENDER
// =========================

void render() {

  display.clearBuffer();

  breatheOffset = sin(millis() * 0.002) * 2;

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

  display.sendBuffer();
}

// =========================
// NORMAL FACE
// =========================

void drawNormalFace() {

  int y = 30 + breatheOffset;

  if(eyesClosed) {

    // closed eyes
    display.drawLine(38, y, 50, y);
    display.drawLine(78, y, 90, y);
  }

  else {

    // open eyes
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

// =========================
// SLEEP FACE
// =========================

void drawSleepFace() {

  int y = 32;

  // sleepy eyes
  display.drawLine(40, y, 50, y);
  display.drawLine(78, y, 88, y);

  // floating zZz
  int zOffset = (millis() / 300) % 10;

  display.setFont(u8g2_font_6x12_tr);

  display.drawStr(58, 20 - zOffset, "z");
  display.drawStr(68, 14 - zOffset, "Z");
}

// =========================
// ANNOYED FACE
// =========================
void drawAnnoyedFace() {

  int y = 30 + breatheOffset;

  // blink especial con mucha luz
  if(eyesClosed) {

    // ojo izquierdo >
    display.drawLine(38, y - 4, 45, y);
    display.drawLine(38, y + 4, 45, y);

    // ojo derecho <
    display.drawLine(90, y - 4, 83, y);
    display.drawLine(90, y + 4, 83, y);
  }

  else {

    // ojos normales
    display.drawDisc(45, y, 5);
    display.drawDisc(83, y, 5);
  }
}
// =========================
// HEART
// =========================
void drawHeart(int x, int y) {

  // parte superior
  display.drawDisc(x - 2, y, 3);
  display.drawDisc(x + 2, y, 3);

  // parte inferior
  display.drawTriangle(
    x - 5, y + 1,
    x + 5, y + 1,
    x, y + 8
  );
}

// =========================
// BOOT SCREEN
// =========================

void bootScreen() {

  // pantallita inicial
  display.clearBuffer();

  display.setFont(u8g2_font_helvB10_tr);

  display.drawStr(18, 28, "by Dai!");

  display.setFont(u8g2_font_helvR10_tr);

  display.drawStr(48, 48, "2026 :D");

  display.sendBuffer();

  delay(2200);

}
