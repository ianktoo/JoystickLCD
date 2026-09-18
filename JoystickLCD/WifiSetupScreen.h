// --- WifiSetupScreen.h ---
// Scans for nearby WiFi networks and lets you browse them with UP/DOWN.
// Entering a password/connecting isn't implemented yet (needs a text-entry
// UI) - this screen currently just surveys what's visible.
#pragma once

#include <WiFiS3.h>
#include "Screen.h"
#include "ScreenManager.h"

class WifiSetupScreen : public Screen {
  private:
    static const int MAX_NETWORKS = 10;
    String ssids[MAX_NETWORKS];
    int networkCount = 0;
    int index = 0;

    void drawNetwork(LiquidCrystal_I2C &lcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi " + String(index + 1) + "/" + String(networkCount));
      lcd.setCursor(0, 1);
      String ssid = ssids[index];
      if (ssid.length() > 16) {
        ssid = ssid.substring(0, 16);
      }
      lcd.print(ssid);
    }

    void drawNoNetworks(LiquidCrystal_I2C &lcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No networks");
      lcd.setCursor(0, 1);
      lcd.print("< LEFT to go back");
    }

  public:
    void enter(LiquidCrystal_I2C &lcd) override {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Scanning WiFi...");

      networkCount = WiFi.scanNetworks();
      if (networkCount > MAX_NETWORKS) {
        networkCount = MAX_NETWORKS;
      }
      for (int i = 0; i < networkCount; i++) {
        ssids[i] = WiFi.SSID(i);
      }
      index = 0;

      if (networkCount <= 0) {
        networkCount = 0;
        drawNoNetworks(lcd);
      } else {
        drawNetwork(lcd);
      }
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      if (direction == "LEFT") {
        manager.goHome(lcd);
        return;
      }
      if (networkCount == 0) {
        return;
      }
      if (direction == "DOWN" && index < networkCount - 1) {
        index++;
        drawNetwork(lcd);
      } else if (direction == "UP" && index > 0) {
        index--;
        drawNetwork(lcd);
      }
    }
};
