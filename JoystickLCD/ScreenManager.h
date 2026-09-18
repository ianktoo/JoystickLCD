// --- ScreenManager.h ---
// Owns the currently active Screen and handles switching between screens.
// Screens call manager.goHome()/goTo() instead of knowing about each other.
#pragma once

#include "Screen.h"
#include "Storage.h"
#include "NullStorage.h"

class ScreenManager {
  private:
    Screen* currentScreen = nullptr;
    Screen* homeScreen = nullptr;
    NullStorage noopStorage;
    Storage* storage = &noopStorage;

  public:
    void setStorage(Storage* s) {
      storage = s;
    }

    void begin(Screen* home, LiquidCrystal_I2C &lcd) {
      homeScreen = home;
      goTo(home, lcd);
    }

    void goTo(Screen* screen, LiquidCrystal_I2C &lcd) {
      currentScreen = screen;
      Serial.print("[Screen] -> ");
      Serial.println(currentScreen->name());
      storage->log(String("screen:") + currentScreen->name());
      currentScreen->enter(lcd);
    }

    void goHome(LiquidCrystal_I2C &lcd) {
      goTo(homeScreen, lcd);
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd) {
      currentScreen->handleInput(direction, lcd, *this);
    }

    void update(LiquidCrystal_I2C &lcd) {
      currentScreen->update(lcd);
    }
};
