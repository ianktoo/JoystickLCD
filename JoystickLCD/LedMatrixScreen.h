// --- LedMatrixScreen.h ---
// Drives the Uno R4 WiFi's built-in 8x12 LED matrix with a random sparkle
// animation, updated on a timer independent of the joystick polling rate.
#pragma once

#include "Arduino_LED_Matrix.h"
#include "Screen.h"
#include "ScreenManager.h"

class LedMatrixScreen : public Screen {
  private:
    static const int FRAME_INTERVAL_MS = 150;

    ArduinoLEDMatrix matrix;
    bool began = false;
    unsigned long lastFrameTime = 0;

    void randomFrame() {
      uint8_t frame[8][12];
      for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 12; x++) {
          frame[y][x] = (random(0, 4) == 0) ? 1 : 0; // ~25% of pixels lit
        }
      }
      matrix.renderBitmap(frame, 8, 12);
    }

  public:
    const char* name() override { return "LedMatrix"; }

    void enter(LiquidCrystal_I2C &lcd) override {
      if (!began) {
        matrix.begin();
        began = true;
        Serial.println("[LedMatrix] initialized");
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LED Matrix:");
      lcd.setCursor(0, 1);
      lcd.print("< LEFT to go back");
      lastFrameTime = 0; // draw a frame immediately
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      if (direction == "LEFT") {
        matrix.clear();
        manager.goHome(lcd);
      }
    }

    void update(LiquidCrystal_I2C &lcd) override {
      if (millis() - lastFrameTime >= FRAME_INTERVAL_MS) {
        lastFrameTime = millis();
        randomFrame();
      }
    }
};
