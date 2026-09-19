// --- Indicator.h ---
// Capability contract for a row of on/off indicator lights. Screens only ever
// talk to this interface, so the hardware behind it (a 74HC595 shift register
// today; an I2C expander or direct GPIO tomorrow) can be swapped without
// touching any screen code.
#pragma once

#include <Arduino.h>

class Indicator {
  public:
    virtual ~Indicator() {}

    // Prepares the hardware. Returns false if it couldn't be set up.
    virtual bool begin() = 0;

    // How many lights this indicator drives (1-8).
    virtual int count() = 0;

    // Sets all lights at once. Bit 0 is the first light; bits beyond count() are ignored.
    virtual void setMask(uint8_t mask) = 0;

    // Turns every light off.
    void clear() { setMask(0); }

    // Lights the first `level` lights (a bar graph). Out-of-range values are clamped.
    void setLevel(int level) {
      level = constrain(level, 0, count());
      setMask((uint8_t)((1 << level) - 1));
    }
};
