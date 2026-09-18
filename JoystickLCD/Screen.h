// --- Screen.h ---
// Contract every menu screen implements. New screens plug into the
// ScreenManager without any changes to the main sketch or other screens.
#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class ScreenManager; // forward declaration to avoid a circular include

class Screen {
  public:
    virtual ~Screen() {}

    // Short name used for Serial debug logging.
    virtual const char* name() { return "Screen"; }

    // Called once when the manager switches to this screen.
    virtual void enter(LiquidCrystal_I2C &lcd) {}

    // Called on every joystick direction change (edge-triggered).
    virtual void handleInput(const String &direction, LiquidCrystal_I2C &lcd, ScreenManager &manager) = 0;

    // Called every loop iteration, for screens that animate (e.g. scrolling text).
    virtual void update(LiquidCrystal_I2C &lcd) {}
};
