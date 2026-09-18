// --- WifiPasswordScreen.h ---
// Joystick text entry (via TextEntryScreen) for a WiFi password, then
// attempts to connect and reports the result on the LCD and Serial.
#pragma once

#include <WiFiS3.h>
#include "TextEntryScreen.h"

class WifiPasswordScreen : public TextEntryScreen {
  private:
    String ssid;

  protected:
    String promptLine() override {
      return "Pwd: " + ssid;
    }

    void onSubmit(const String &password, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      Serial.print("[WifiPassword] connecting to ");
      Serial.print(ssid);
      Serial.print(" (password length ");
      Serial.print(password.length());
      Serial.println(")");

      lcd.noBlink();
      lcd.noCursor();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connecting...");
      lcd.setCursor(0, 1);
      String label = ssid;
      if (label.length() > 16) {
        label = label.substring(0, 16);
      }
      lcd.print(label);

      WiFi.begin(ssid.c_str(), password.c_str());

      const unsigned long timeoutMs = 15000;
      unsigned long start = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
        delay(500);
        Serial.print(".");
      }
      Serial.println();

      lcd.clear();
      lcd.setCursor(0, 0);
      if (WiFi.status() == WL_CONNECTED) {
        Serial.print("[WifiPassword] connected, IP = ");
        Serial.println(WiFi.localIP());
        lcd.print("Connected!");
        lcd.setCursor(0, 1);
        lcd.print(WiFi.localIP());
      } else {
        Serial.print("[WifiPassword] connect failed, status = ");
        Serial.println(WiFi.status());
        lcd.print("Connect failed");
        lcd.setCursor(0, 1);
        lcd.print("< LEFT to retry");
      }
      showingResult = true;
    }

  public:
    void setSsid(const String &newSsid) {
      ssid = newSsid;
    }

    const char* name() override { return "WifiPassword"; }
};
