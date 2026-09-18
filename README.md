# JoystickLCD

A work-in-progress Arduino project for the **Uno R4 WiFi** that drives a 16x2 I2C LCD menu using a Freenove analog joystick module.

## Hardware

- Arduino Uno R4 WiFi
- Freenove joystick module (analog X/Y + push button)
- 16x2 I2C LCD (LiquidCrystal_I2C, address `0x27`)
- Breadboard + jumper wires

## Wiring

| Joystick pin | Arduino pin |
|---|---|
| VRx | A0 |
| VRy | A1 |
| SW  | D2 |
| VCC | 5V |
| GND | GND |

LCD connects via I2C (SDA/SCL) plus 5V/GND.

## Features

- Joystick-driven main menu (up/down to navigate, press to select)
- Submenu screens (WiFi Setup, Settings, Device Info)
- Scrolling text helper (`Scroller.h`) for long strings on a 16-char display

## Status

Early work in progress — menu navigation and scrolling text work; WiFi setup and settings screens are placeholders.

## Dependencies

- [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C) (install via Arduino Library Manager)

## License

MIT
