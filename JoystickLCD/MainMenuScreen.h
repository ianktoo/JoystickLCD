// --- MainMenuScreen.h ---
// Generic scrollable menu: UP/DOWN to move, PRESSED to enter the selected
// screen. Takes its labels/targets as constructor arguments, so it isn't
// tied to any specific set of menu items.
#pragma once

#include "Screen.h"
#include "ScreenManager.h"

class MainMenuScreen : public Screen {
  private:
    String* labels;
    Screen** targets;
    int count;
    int index = 0;

    void draw(LiquidCrystal_I2C &lcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Main Menu:");
      lcd.setCursor(0, 1);
      lcd.print(labels[index]);
    }

  public:
    MainMenuScreen(String labelsArr[], Screen* targetsArr[], int itemCount)
      : labels(labelsArr), targets(targetsArr), count(itemCount) {}

    void enter(LiquidCrystal_I2C &lcd) override {
      index = 0;
      draw(lcd);
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      if (direction == "DOWN" && index < count - 1) {
        index++;
        draw(lcd);
      } else if (direction == "UP" && index > 0) {
        index--;
        draw(lcd);
      } else if (direction == "PRESSED") {
        manager.goTo(targets[index], lcd);
      }
    }
};
