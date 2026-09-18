#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Menu.h" // Includes the new file we just created

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int JOY_VRX_PIN = A0; 
const int JOY_VRY_PIN = A1; 
const int JOY_SW_PIN = 2; 

void setup() {
  Serial.begin(115200);
  pinMode(JOY_SW_PIN, INPUT_PULLUP); 

  lcd.init();
  lcd.backlight();
  
  // Display the initial menu state
  lcd.setCursor(0, 0);
  lcd.print("Main Menu:");
  lcd.setCursor(0, 1);
  lcd.print(menuItems[menuIndex]); // This variable comes from Menu.h
}

void loop() {
  int xPosition = analogRead(JOY_VRX_PIN); 
  int yPosition = analogRead(JOY_VRY_PIN); 
  int buttonState = digitalRead(JOY_SW_PIN); 

  String currentDirection = "CENTER";

  // Determine direction
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

  // Call the function from Menu.h and pass it the direction and the LCD
  handleMenu(currentDirection, lcd);

  delay(50);
}