// --- AboutScreen.h ---
// Shows project/version/author info, scrolled via the reusable TextScroller.
#pragma once

#include "Screen.h"
#include "ScreenManager.h"
#include "Scroller.h"

class AboutScreen : public Screen {
  private:
    TextScroller scroller;

  public:
    AboutScreen()
      : scroller("JoystickLCD v0.1 by Ian T. - github.com/ianktoo/JoystickLCD", 1, 300) {}

    void enter(LiquidCrystal_I2C &lcd) override {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("About:");
      scroller.reset();
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      if (direction == "LEFT") {
        manager.goHome(lcd);
      }
    }

    void update(LiquidCrystal_I2C &lcd) override {
      scroller.update(lcd);
    }
};
