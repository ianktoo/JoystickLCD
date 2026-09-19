// --- LedBarScreen.h ---
// Drives an external row of LEDs through the Indicator contract. UP/DOWN cycles
// between animations; in Level mode, PRESSED steps the bar up one light at a
// time. LEFT goes back to the main menu.
#pragma once

#include "Screen.h"
#include "ScreenManager.h"
#include "Indicator.h"

class LedBarScreen : public Screen {
  private:
    static const int STEP_INTERVAL_MS = 200;
    static const int MODE_COUNT = 4;
    static const int MODE_LEVEL = 3;

    Indicator* indicator;
    int mode = 0;
    int step = 0;
    int stepDirection = 1;
    int level = 0;
    unsigned long lastStepTime = 0;

    const char* modeName() {
      switch (mode) {
        case 0: return "Chase";
        case 1: return "Bounce";
        case 2: return "Counter";
        case 3: return "Level";
      }
      return "?";
    }

    void drawLabel(LiquidCrystal_I2C &lcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(String("LED Bar: ") + modeName());
      lcd.setCursor(0, 1);
      if (mode == MODE_LEVEL) {
        lcd.print(String("PRESS: level ") + level);
      } else {
        lcd.print("<back  UP/DN:next");
      }
    }

    void resetState() {
      step = 0;
      stepDirection = 1;
      level = 0;
      indicator->clear();
    }

    void renderStep() {
      int lights = indicator->count();
      switch (mode) {
        case 0: // Chase: one light travelling in a loop
          indicator->setMask((uint8_t)(1 << step));
          step = (step + 1) % lights;
          break;
        case 1: // Bounce: one light travelling back and forth
          indicator->setMask((uint8_t)(1 << step));
          if (lights > 1) {
            if (step <= 0) stepDirection = 1;
            else if (step >= lights - 1) stepDirection = -1;
            step += stepDirection;
          }
          break;
        case 2: // Counter: lights show the step number in binary
          indicator->setMask((uint8_t)step);
          step = (step + 1) % (1 << lights);
          break;
      }
    }

  public:
    LedBarScreen(Indicator* indicatorPtr) : indicator(indicatorPtr) {}

    const char* name() override { return "LedBar"; }

    void enter(LiquidCrystal_I2C &lcd) override {
      resetState();
      drawLabel(lcd);
      lastStepTime = 0; // draw a step immediately
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      if (direction == "LEFT") {
        indicator->clear();
        manager.goHome(lcd);
      } else if (direction == "UP" || direction == "DOWN") {
        mode = (mode + 1) % MODE_COUNT;
        resetState();
        Serial.print("[LedBar] mode -> ");
        Serial.println(modeName());
        drawLabel(lcd);
        lastStepTime = 0;
      } else if (direction == "PRESSED" && mode == MODE_LEVEL) {
        level = (level + 1) % (indicator->count() + 1);
        indicator->setLevel(level);
        drawLabel(lcd);
      }
    }

    void update(LiquidCrystal_I2C &lcd) override {
      if (mode == MODE_LEVEL) return; // static; only changes on input
      if (millis() - lastStepTime >= STEP_INTERVAL_MS) {
        lastStepTime = millis();
        renderStep();
      }
    }
};
