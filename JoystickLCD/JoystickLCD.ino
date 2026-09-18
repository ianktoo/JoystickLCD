#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "Screen.h"
#include "ScreenManager.h"
#include "Storage.h"
#include "NullStorage.h"
#include "SdStorage.h"
#include "MainMenuScreen.h"
#include "WifiSetupScreen.h"
#include "WifiPasswordScreen.h"
#include "SiteCheckScreen.h"
#include "LedMatrixScreen.h"
#include "SettingsScreen.h"
#include "AboutScreen.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int JOY_VRX_PIN = A0;
const int JOY_VRY_PIN = A1;
const int JOY_SW_PIN = 2;
const int RANDOM_SEED_PIN = A2; // left floating; only used to seed random()

// Chip-select pin for an SD card module, if one is wired up over SPI.
// If no card is present, SdStorage.begin() just fails and the sketch
// falls back to NullStorage (Serial-only logging, nothing crashes).
const int SD_CS_PIN = 4;

ScreenManager screenManager;

SdStorage sdStorage(SD_CS_PIN);
NullStorage nullStorage;
Storage* storage = &nullStorage;

WifiSetupScreen wifiSetupScreen;
WifiPasswordScreen wifiPasswordScreen;
SiteCheckScreen siteCheckScreen;
LedMatrixScreen ledMatrixScreen;
SettingsScreen settingsScreen;
AboutScreen aboutScreen;

String menuLabels[] = { "1. WiFi Setup", "2. Site Check", "3. LED Matrix", "4. Settings", "5. About" };
Screen* menuTargets[] = { &wifiSetupScreen, &siteCheckScreen, &ledMatrixScreen, &settingsScreen, &aboutScreen };
MainMenuScreen mainMenuScreen(menuLabels, menuTargets, 5);

String lastDirection = "CENTER";

void setup() {
  Serial.begin(115200);
  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  randomSeed(analogRead(RANDOM_SEED_PIN));

  lcd.init();
  lcd.backlight();

  if (sdStorage.begin()) {
    storage = &sdStorage;
  } else {
    nullStorage.begin();
  }
  storage->log("boot");

  screenManager.setStorage(storage);
  wifiSetupScreen.setPasswordScreen(&wifiPasswordScreen);
  wifiPasswordScreen.setBackTarget(&wifiSetupScreen);
  siteCheckScreen.setBackTarget(&mainMenuScreen);
  settingsScreen.setStorage(storage);

  bool backlightOn = storage->loadSetting("backlight", "on") != "off";
  settingsScreen.applyBacklight(backlightOn, lcd);

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
      Serial.print("[Input] ");
      Serial.println(currentDirection);
      screenManager.handleInput(currentDirection, lcd);
    }
    lastDirection = currentDirection;
  }

  screenManager.update(lcd);

  delay(50);
}
