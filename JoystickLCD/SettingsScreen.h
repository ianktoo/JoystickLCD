// --- SettingsScreen.h ---
// Placeholder settings screen.
#pragma once

#include "Screen.h"
#include "ScreenManager.h"

class SettingsScreen : public Screen {
  public:
    void enter(LiquidCrystal_I2C &lcd) override {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Settings Screen");
      lcd.setCursor(0, 1);
      lcd.print("< LEFT to go back");
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      if (direction == "LEFT") {
        manager.goHome(lcd);
      }
    }
};
