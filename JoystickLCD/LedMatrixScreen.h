// --- LedMatrixScreen.h ---
// Drives the Uno R4 WiFi's built-in 8x12 LED matrix. UP/DOWN cycles between
// a few different visualizations; LEFT goes back to the main menu.
#pragma once

#include <math.h>
#include "Arduino_LED_Matrix.h"
#include "Screen.h"
#include "ScreenManager.h"

class LedMatrixScreen : public Screen {
  private:
    static const int FRAME_INTERVAL_MS = 150;
    static const int PATTERN_COUNT = 4;
    static const int ROWS = 8;
    static const int COLS = 12;

    ArduinoLEDMatrix matrix;
    bool began = false;
    unsigned long lastFrameTime = 0;
    int patternIndex = 0;

    // Rain pattern state: current lit row per column.
    int dropRow[COLS];

    // Rings pattern state: current ring radius.
    int ringRadius = 0;

    // Bounce pattern state: single pixel position/velocity.
    int bounceX = 0, bounceY = 0, bounceDX = 1, bounceDY = 1;

    const char* patternName() {
      switch (patternIndex) {
        case 0: return "Sparkle";
        case 1: return "Rain";
        case 2: return "Rings";
        case 3: return "Bounce";
      }
      return "?";
    }

    void drawLabel(LiquidCrystal_I2C &lcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(String("Matrix: ") + patternName());
      lcd.setCursor(0, 1);
      lcd.print("<back  UP/DN:next");
    }

    void resetPatternState() {
      for (int x = 0; x < COLS; x++) {
        dropRow[x] = random(0, ROWS);
      }
      ringRadius = 0;
      bounceX = 0;
      bounceY = 0;
      bounceDX = 1;
      bounceDY = 1;
    }

    void renderSparkle(uint8_t frame[ROWS][COLS]) {
      for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
          frame[y][x] = (random(0, 4) == 0) ? 1 : 0;
        }
      }
    }

    void renderRain(uint8_t frame[ROWS][COLS]) {
      for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
          frame[y][x] = 0;
        }
      }
      for (int x = 0; x < COLS; x++) {
        frame[dropRow[x]][x] = 1;
        dropRow[x] = (dropRow[x] + 1) % ROWS;
        if (random(0, 20) == 0) {
          dropRow[x] = 0; // occasionally restart a column early for variety
        }
      }
    }

    void renderRings(uint8_t frame[ROWS][COLS]) {
      const float cx = (COLS - 1) / 2.0f;
      const float cy = (ROWS - 1) / 2.0f;
      for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
          float dist = sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
          frame[y][x] = (fabs(dist - ringRadius) < 0.8f) ? 1 : 0;
        }
      }
      ringRadius = (ringRadius + 1) % ROWS;
    }

    void renderBounce(uint8_t frame[ROWS][COLS]) {
      for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
          frame[y][x] = 0;
        }
      }
      frame[bounceY][bounceX] = 1;
      bounceX += bounceDX;
      bounceY += bounceDY;
      if (bounceX <= 0 || bounceX >= COLS - 1) {
        bounceDX = -bounceDX;
      }
      if (bounceY <= 0 || bounceY >= ROWS - 1) {
        bounceDY = -bounceDY;
      }
      bounceX = constrain(bounceX, 0, COLS - 1);
      bounceY = constrain(bounceY, 0, ROWS - 1);
    }

    void renderFrame() {
      uint8_t frame[ROWS][COLS];
      switch (patternIndex) {
        case 0: renderSparkle(frame); break;
        case 1: renderRain(frame); break;
        case 2: renderRings(frame); break;
        case 3: renderBounce(frame); break;
      }
      matrix.renderBitmap(frame, ROWS, COLS);
    }

  public:
    const char* name() override { return "LedMatrix"; }

    void enter(LiquidCrystal_I2C &lcd) override {
      if (!began) {
        matrix.begin();
        began = true;
        Serial.println("[LedMatrix] initialized");
      }
      resetPatternState();
      drawLabel(lcd);
      lastFrameTime = 0; // draw a frame immediately
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      if (direction == "LEFT") {
        matrix.clear();
        manager.goHome(lcd);
      } else if (direction == "UP" || direction == "DOWN") {
        patternIndex = (patternIndex + 1) % PATTERN_COUNT;
        resetPatternState();
        Serial.print("[LedMatrix] pattern -> ");
        Serial.println(patternName());
        drawLabel(lcd);
      }
    }

    void update(LiquidCrystal_I2C &lcd) override {
      if (millis() - lastFrameTime >= FRAME_INTERVAL_MS) {
        lastFrameTime = millis();
        renderFrame();
      }
    }
};
