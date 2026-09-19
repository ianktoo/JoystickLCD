// --- ShiftRegisterIndicator.h ---
// Indicator adapter for a 74HC595 shift register. Bit 0 lands on output QA
// (pin 15), bit 1 on QB, and so on, because bits are shifted out MSB first.
// Wiring: see docs/schematic-595.svg and docs/breadboard-595.svg.
#pragma once

#include "Indicator.h"

class ShiftRegisterIndicator : public Indicator {
  private:
    int dataPin;   // SER   (74HC595 pin 14)
    int clockPin;  // SRCLK (74HC595 pin 11)
    int latchPin;  // RCLK  (74HC595 pin 12)
    int lights;
    uint8_t validMask;

  public:
    ShiftRegisterIndicator(int data, int clock, int latch, int lightCount = 8)
      : dataPin(data), clockPin(clock), latchPin(latch) {
      lights = constrain(lightCount, 1, 8);
      validMask = (uint8_t)((1 << lights) - 1);
    }

    bool begin() override {
      pinMode(dataPin, OUTPUT);
      pinMode(clockPin, OUTPUT);
      pinMode(latchPin, OUTPUT);
      setMask(0);
      return true;
    }

    int count() override { return lights; }

    void setMask(uint8_t mask) override {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, mask & validMask);
      digitalWrite(latchPin, HIGH);
    }
};
