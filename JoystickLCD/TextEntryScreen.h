// --- TextEntryScreen.h ---
// Reusable joystick text editor. Subclasses only provide a prompt line and
// what to do with the submitted text - the character-cycling UI, cursor,
// and result/back-navigation handling are shared.
//
// Controls while editing:
//   UP/DOWN   cycle the character at the cursor through the charset
//   RIGHT     commit that character and move to the next position
//   LEFT      backspace, or (on empty input) cancel back to backTarget
//   PRESSED   submit the text typed so far via onSubmit()
//
// Subclasses that want to show a result screen after onSubmit() should set
// showingResult = true; any input then returns to backTarget.
#pragma once

#include "Screen.h"
#include "ScreenManager.h"

class TextEntryScreen : public Screen {
  protected:
    static const char* CHARSET;
    static const int MAX_LEN = 32;

    String buffer;
    int charIndex = 0;
    bool showingResult = false;
    Screen* backTarget = nullptr;

    int charsetLength() {
      return strlen(CHARSET);
    }

    virtual String promptLine() = 0;
    virtual void onSubmit(const String &text, LiquidCrystal_I2C &lcd, ScreenManager &manager) = 0;

    void draw(LiquidCrystal_I2C &lcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      String label = promptLine();
      if (label.length() > 16) {
        label = label.substring(0, 16);
      }
      lcd.print(label);

      String display = buffer;
      display += CHARSET[charIndex];
      if (display.length() > 16) {
        display = display.substring(display.length() - 16);
      }
      lcd.setCursor(0, 1);
      lcd.print(display);
      lcd.setCursor(display.length() - 1, 1);
      lcd.cursor();
      lcd.blink();
    }

    void exitToBack(LiquidCrystal_I2C &lcd, ScreenManager &manager) {
      lcd.noBlink();
      lcd.noCursor();
      manager.goTo(backTarget, lcd);
    }

  public:
    void setBackTarget(Screen* target) {
      backTarget = target;
    }

    void enter(LiquidCrystal_I2C &lcd) override {
      buffer = "";
      charIndex = 0;
      showingResult = false;
      draw(lcd);
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      if (showingResult) {
        if (direction == "LEFT" || direction == "PRESSED") {
          exitToBack(lcd, manager);
        }
        return;
      }

      if (direction == "LEFT") {
        if (buffer.length() > 0) {
          buffer.remove(buffer.length() - 1);
          charIndex = 0;
          draw(lcd);
        } else {
          exitToBack(lcd, manager);
        }
      } else if (direction == "RIGHT" && buffer.length() < MAX_LEN) {
        buffer += CHARSET[charIndex];
        charIndex = 0;
        draw(lcd);
      } else if (direction == "UP") {
        charIndex = (charIndex + 1) % charsetLength();
        draw(lcd);
      } else if (direction == "DOWN") {
        charIndex = (charIndex - 1 + charsetLength()) % charsetLength();
        draw(lcd);
      } else if (direction == "PRESSED") {
        String finalText = buffer;
        finalText += CHARSET[charIndex];
        onSubmit(finalText, lcd, manager);
      }
    }
};

const char* TextEntryScreen::CHARSET =
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*-_.:/";
