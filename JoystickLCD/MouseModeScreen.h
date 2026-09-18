// --- MouseModeScreen.h ---
// Turns the joystick into a USB HID mouse: analog deflection moves the OS
// cursor, the button clicks. This is a one-way trip - once Mouse.begin() is
// called the joystick is fully committed to cursor movement, so there is no
// menu input left to navigate back out with. Reset the board or unplug it
// from power to return to the normal menu.
#pragma once

#include <Mouse.h>
#include "Screen.h"
#include "ScreenManager.h"

class MouseModeScreen : public Screen {
  private:
    static const int DEADZONE = 80;   // ignore small drift around center
    static const int MAX_STEP = 8;    // max cursor pixels moved per tick
    static const int CENTER = 512;

    int vrxPin = -1;
    int vryPin = -1;
    int swPin = -1;
    bool started = false;
    bool buttonDown = false;

    int axisStep(int raw) {
      int offset = raw - CENTER;
      if (abs(offset) < DEADZONE) {
        return 0;
      }
      int magnitude = map(abs(offset), DEADZONE, CENTER, 1, MAX_STEP);
      magnitude = constrain(magnitude, 1, MAX_STEP);
      return offset > 0 ? magnitude : -magnitude;
    }

  public:
    // Pins are injected rather than hardcoded, so this screen doesn't need
    // to know about the rest of the sketch's wiring constants.
    void setJoystickPins(int vrx, int vry, int sw) {
      vrxPin = vrx;
      vryPin = vry;
      swPin = sw;
    }

    const char* name() override { return "MouseMode"; }

    void enter(LiquidCrystal_I2C &lcd) override {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mouse Mode ON");
      lcd.setCursor(0, 1);
      lcd.print("Reset to undo");

      if (!started) {
        Mouse.begin();
        started = true;
        Serial.println("[MouseMode] Mouse.begin() - joystick now drives the OS cursor; reset/unplug to return to the menu");
      }
    }

    void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) override {
      // Intentionally ignored - see file header. The joystick belongs to
      // the OS cursor now, not to menu navigation.
    }

    void update(LiquidCrystal_I2C &lcd) override {
      if (vrxPin < 0) {
        return;
      }

      int dx = axisStep(analogRead(vrxPin));
      int dy = axisStep(analogRead(vryPin));
      if (dx != 0 || dy != 0) {
        Mouse.move(dx, dy, 0);
      }

      bool pressed = digitalRead(swPin) == LOW;
      if (pressed && !buttonDown) {
        Mouse.press();
        buttonDown = true;
      } else if (!pressed && buttonDown) {
        Mouse.release();
        buttonDown = false;
      }
    }
};
