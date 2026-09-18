// --- SiteCheckScreen.h ---
// Type a hostname (via TextEntryScreen) and check whether it responds with
// 200, 404, needs auth (401/403), or doesn't respond at all. Useful as a
// quick "is the network actually working" check once WiFi is connected.
#pragma once

#include "TextEntryScreen.h"
#include "HttpStatusChecker.h"

class SiteCheckScreen : public TextEntryScreen {
  protected:
    String promptLine() override {
      return "Enter host:";
    }

    void onSubmit(const String &host, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      lcd.noBlink();
      lcd.noCursor();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Checking...");
      String shown = host;
      if (shown.length() > 16) {
        shown = shown.substring(0, 16);
      }
      lcd.setCursor(0, 1);
      lcd.print(shown);

      HttpCheckResult result = HttpStatusChecker::check(host);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(shown);
      lcd.setCursor(0, 1);
      if (!result.reached) {
        lcd.print("No response");
      } else if (result.statusCode == 200) {
        lcd.print("200 OK");
      } else if (result.statusCode == 404) {
        lcd.print("404 Not Found");
      } else if (result.statusCode == 401 || result.statusCode == 403) {
        lcd.print("Auth needed");
      } else if (result.statusCode > 0) {
        lcd.print("Status " + String(result.statusCode));
      } else {
        lcd.print("Bad response");
      }
      showingResult = true;
    }

  public:
    const char* name() override { return "SiteCheck"; }
};
