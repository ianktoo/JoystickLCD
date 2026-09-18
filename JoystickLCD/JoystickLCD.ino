#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Screen.h"
#include "ScreenManager.h"
#include "MainMenuScreen.h"
#include "WifiSetupScreen.h"
#include "SettingsScreen.h"
#include "AboutScreen.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int JOY_VRX_PIN = A0;
const int JOY_VRY_PIN = A1;
const int JOY_SW_PIN = 2;

ScreenManager screenManager;

WifiSetupScreen wifiSetupScreen;
SettingsScreen settingsScreen;
AboutScreen aboutScreen;

String menuLabels[] = { "1. WiFi Setup", "2. Settings", "3. About" };
Screen* menuTargets[] = { &wifiSetupScreen, &settingsScreen, &aboutScreen };
MainMenuScreen mainMenuScreen(menuLabels, menuTargets, 3);

String lastDirection = "CENTER";

void setup() {
  Serial.begin(115200);
  pinMode(JOY_SW_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  screenManager.begin(&mainMenuScreen, lcd);
}

void loop() {
  int xPosition = analogRead(JOY_VRX_PIN);
  int yPosition = analogRead(JOY_VRY_PIN);
  int buttonState = digitalRead(JOY_SW_PIN);

  String currentDirection = "CENTER";

  if (xPosition < 200) {
    currentDirection = "LEFT";
  } else if (xPosition > 800) {
    currentDirection = "RIGHT";
  } else if (yPosition < 200) {
    currentDirection = "UP";
  } else if (yPosition > 800) {
    currentDirection = "DOWN";
  }

  if (buttonState == LOW) {
    currentDirection = "PRESSED";
  }

  // Edge-trigger on direction changes, and re-arm once the stick returns to
  // CENTER so holding/repeating the same direction works.
  if (currentDirection != lastDirection) {
    if (currentDirection != "CENTER") {
      screenManager.handleInput(currentDirection, lcd);
    }
    lastDirection = currentDirection;
  }

  screenManager.update(lcd);

  delay(50);
}
