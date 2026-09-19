# JoystickLCD

A work-in-progress Arduino project for the **Uno R4 WiFi** that drives a 16x2 I2C LCD menu using a Freenove analog joystick module, with WiFi setup, a quick site-status checker, and the board's built-in LED matrix put to use.

## Hardware

- Arduino Uno R4 WiFi
- Freenove joystick module (analog X/Y + push button)
- 16x2 I2C LCD (LiquidCrystal_I2C, address `0x27`)
- Breadboard + jumper wires
- *(optional)* SD card module, for on-device settings/log persistence
- *(optional)* SN74HC595N shift register + LEDs (each with a 220 ohm resistor), for the external LED bar

## Schematic

![Wiring schematic](docs/schematic.svg)

LED bar (74HC595): [circuit schematic](docs/schematic-595.svg) and [breadboard layout](docs/breadboard-595.svg).

## Wiring

| Joystick pin | Arduino pin |
|---|---|
| VRx | A0 |
| VRy | A1 |
| SW  | D2 |
| VCC | 5V |
| GND | GND |

LCD connects via I2C (SDA/SCL) plus 5V/GND.

*(Optional)* SD card module: CS -> D4, MOSI/MISO/SCK -> the R4's SPI pins (D11-D13), VCC/GND shared with the rest of the circuit. If no SD module is wired up, the sketch detects that at boot and just logs to Serial instead - nothing else changes.

*(Optional)* 74HC595 LED bar: D7 -> SER (pin 14), D5 -> SRCLK (pin 11), D6 -> RCLK (pin 12), 5V -> VCC (pin 16) and SRCLR (pin 10), GND -> GND (pin 8) and OE (pin 13). Outputs QA, QB, QC... each go through a 220 ohm resistor to an LED, then to GND. Set `INDICATOR_LED_COUNT` in `JoystickLCD.ino` to the number of LEDs you wired. With nothing connected the LED Bar screen simply lights nothing; the rest of the menu is unaffected.

## Features

- Joystick-driven main menu (UP/DOWN to navigate, PRESSED to select, LEFT to go back)
- **WiFi Setup** - scans nearby networks, browse with UP/DOWN, then type a password with the joystick (UP/DOWN cycles the character, RIGHT commits it and advances, LEFT backspaces) and connect
- **Site Check** - type a hostname the same way, and it reports back 200 OK / 404 Not Found / "Auth needed" (401/403) / no response - a quick way to sanity-check the WiFi connection actually reaches the internet
- **LED Matrix** - drives the Uno R4 WiFi's onboard 8x12 LED matrix; UP/DOWN cycles between four visualizations (Sparkle, Rain, Rings, Bounce)
- **LED Bar** - drives an external row of LEDs through a 74HC595 shift register; UP/DOWN cycles between Chase, Bounce, binary Counter and Level (PRESSED steps the bar up one LED at a time)
- **Mouse Mode** - turns the joystick into a USB HID mouse (analog deflection moves the OS cursor, the button clicks). This is one-way: once active there's no joystick input left over to navigate back out with, so **reset the board or unplug it from power** to return to the menu
- **Settings** - toggle the LCD backlight (the only thing this I2C backpack exposes in software; contrast is a physical trim-pot), and re-probe for an SD card on demand (hot-swaps persistence over to it if one is found, no reboot needed)
- **About** - project/version info via the reusable scrolling-text helper
- Persistent settings and an on-device log file when an SD card module is present, via a swappable storage layer (falls back to Serial-only logging otherwise)
- Verbose Serial output (115200 baud) for debugging: screen transitions, joystick input, WiFi scan/connect results, HTTP checks

## Design

Menu screens share one small contract (`Screen.h`): `enter()`, `handleInput()`, `update()`. `ScreenManager` swaps between them, so adding a new screen doesn't touch the main sketch or any other screen. Joystick-driven text entry (used by both WiFi Setup and Site Check) is factored into `TextEntryScreen`, a reusable base class. Persistence goes through a `Storage` contract (`SdStorage` / `NullStorage`), so the rest of the code never has to ask "is there an SD card?". The external LEDs work the same way: screens talk only to the `Indicator` contract (`Indicator.h`), and `ShiftRegisterIndicator` is the 74HC595 adapter behind it, so the hardware can change without touching any screen.

## Tests

`tests/contracts/ShiftRegisterIndicatorTest` is a standalone hardware test for the 74HC595 adapter: it checks `begin()`/`count()` over Serial and walks the LEDs so you can confirm the wiring by eye. The builder can't include headers from a parent folder, so give it the adapter folder as an include path (use a path without spaces, or the Windows short path):

```
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi \
  --build-property "compiler.cpp.extra_flags=-I<path-to>/JoystickLCD/JoystickLCD" \
  tests/contracts/ShiftRegisterIndicatorTest
```

In the Arduino IDE, copy `Indicator.h` and `ShiftRegisterIndicator.h` next to the test `.ino` instead.

## Status

Work in progress. Menu navigation, WiFi scan/connect, site status checks, the LED matrix visualizations, and backlight toggle all work. The LED Bar screen and its adapter compile cleanly but haven't been run against the physical 74HC595 wiring yet. Known limitations: WPA2 passwords are entered one character at a time via joystick (slow, but functional); Mouse Mode compiles against the Uno R4 WiFi's native USB HID support but hasn't been confirmed on real hardware yet; the SD storage layer compiles cleanly but hasn't been tested against a physical card - use Settings -> SD Card to check whether one is detected.

## Debugging

Open the Serial Monitor at **115200 baud** to see screen transitions, joystick input, WiFi scan results, connection attempts, and HTTP status checks as they happen.

## Dependencies

Install via Arduino Library Manager (or `arduino-cli lib install`):

- [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)
- `SD` (Arduino's official SD library - only needed if you wire up an SD card module)

`WiFiS3` and `Arduino_LED_Matrix` ship with the Uno R4 board core, no separate install needed.

## License

MIT
