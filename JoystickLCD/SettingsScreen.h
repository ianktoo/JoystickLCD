// --- SettingsScreen.h ---
// Toggle the LCD backlight (the only software-adjustable setting this
// PCF8574 I2C backpack exposes - contrast is a physical trim-pot). The
// choice is persisted via Storage so it survives a power cycle when an
// SD card is present.
#pragma once

#include "Screen.h"
#include "ScreenManager.h"
#include "Storage.h"
#include "NullStorage.h"

class SettingsScreen : public Screen {
  private:
    NullStorage noopStorage;
    Storage* storage = &noopStorage;
    bool backlightOn = true;

    void draw(LiquidCrystal_I2C &lcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      lcd.print(String("Backlight: ") + (backlightOn ? "ON" : "OFF"));
    }

  public:
    void setStorage(Storage* s) {
      storage = s;
    }

    // Applies a previously-saved state at boot, without re-saving it.
    void applyBacklight(bool on, LiquidCrystal_I2C &lcd) {
      backlightOn = on;
      if (backlightOn) {
        lcd.backlight();
      } else {
        lcd.noBacklight();
      }
    }

    const char* name() override { return "Settings"; }

    void enter(LiquidCrystal_I2C &lcd) override {
      draw(lcd);
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      if (direction == "LEFT") {
        manager.goHome(lcd);
      } else if (direction == "PRESSED" || direction == "UP" || direction == "DOWN") {
        applyBacklight(!backlightOn, lcd);
        Serial.print("[Settings] backlight ");
        Serial.println(backlightOn ? "ON" : "OFF");
        storage->saveSetting("backlight", backlightOn ? "on" : "off");
        draw(lcd);
      }
    }
};
