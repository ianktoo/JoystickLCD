// --- ShiftRegisterIndicatorTest.ino ---
// Hardware contract test for ShiftRegisterIndicator (the 74HC595 adapter).
// Upload this on its own to check your wiring before using the LED Bar menu
// screen. Open Serial Monitor at 115200 baud.
//
// What you should SEE, in order, repeating:
//   1. Chase   - one LED travels QA -> QB -> QC (-> ...) once
//   2. Level   - bar fills one LED at a time, then empties
//   3. Clamp   - setLevel(99) lights ALL LEDs (out-of-range is clamped)
//   4. Clear   - all LEDs off
//
// If an LED never lights, or lights out of order, see docs/breadboard-595.svg.
#include "ShiftRegisterIndicator.h"

const int SR_DATA_PIN = 7;
const int SR_CLOCK_PIN = 5;
const int SR_LATCH_PIN = 6;
const int LED_COUNT = 3; // change to the number of LEDs you have wired

ShiftRegisterIndicator ledBar(SR_DATA_PIN, SR_CLOCK_PIN, SR_LATCH_PIN, LED_COUNT);
Indicator* indicator = &ledBar; // exercise it through the contract, like the screens do

int failures = 0;

void check(const char* what, bool ok) {
  Serial.print(ok ? "[PASS] " : "[FAIL] ");
  Serial.println(what);
  if (!ok) failures++;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("ShiftRegisterIndicator contract test");

  check("begin() returns true", indicator->begin());
  check("count() matches configured LED count", indicator->count() == LED_COUNT);

  Serial.println(failures == 0 ? "Logic checks passed. Watch the LEDs to confirm wiring." : "Logic checks FAILED.");
}

void loop() {
  int lights = indicator->count();

  Serial.println("Chase");
  for (int i = 0; i < lights; i++) {
    indicator->setMask((uint8_t)(1 << i));
    delay(300);
  }

  Serial.println("Level");
  for (int level = 0; level <= lights; level++) {
    indicator->setLevel(level);
    delay(300);
  }

  Serial.println("Clamp (setLevel(99) should light all)");
  indicator->setLevel(99);
  delay(800);

  Serial.println("Clear");
  indicator->clear();
  delay(800);
}
