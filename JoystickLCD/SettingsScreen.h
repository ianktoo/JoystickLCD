// --- SettingsScreen.h ---
// A small settings list (UP/DOWN to move between entries, PRESSED to act):
//   - Backlight: the only thing this PCF8574 I2C backpack exposes in
//     software (contrast is a physical trim-pot). Persisted via Storage.
//   - SD Card: re-probes for a card right now and reports found/not found.
//     On success, it also hot-swaps the active Storage to the SD card, so
//     you don't have to reboot after plugging one in.
#pragma once

#include "Screen.h"
#include "ScreenManager.h"
#include "Storage.h"
#include "NullStorage.h"
#include "SdStorage.h"

class SettingsScreen : public Screen {
  private:
    static const int ITEM_COUNT = 2;

    NullStorage noopStorage;
    Storage* storage = &noopStorage;
    SdStorage* sdCard = nullptr;

    int index = 0;
    bool backlightOn = true;
    String sdStatusLine = "not checked yet";

    void draw(LiquidCrystal_I2C &lcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      if (index == 0) {
        lcd.print(String("Backlight: ") + (backlightOn ? "ON" : "OFF"));
      } else {
        lcd.print("SD: " + sdStatusLine);
      }
    }

  public:
    void setStorage(Storage* s) {
      storage = s;
    }

    void setSdCard(SdStorage* card) {
      sdCard = card;
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
      } else if (direction == "UP" || direction == "DOWN") {
        index = (index + 1) % ITEM_COUNT;
        draw(lcd);
      } else if (direction == "PRESSED") {
        if (index == 0) {
          applyBacklight(!backlightOn, lcd);
          Serial.print("[Settings] backlight ");
          Serial.println(backlightOn ? "ON" : "OFF");
          storage->saveSetting("backlight", backlightOn ? "on" : "off");
        } else if (sdCard != nullptr) {
          lcd.setCursor(0, 1);
          lcd.print("SD: checking...  ");
          bool found = sdCard->begin();
          sdStatusLine = found ? "found" : "not found";
          Serial.print("[Settings] SD card check: ");
          Serial.println(sdStatusLine);
          if (found) {
            storage = sdCard;
            manager.setStorage(sdCard);
          }
        }
        draw(lcd);
      }
    }
};
