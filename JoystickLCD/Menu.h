// --- Menu.h ---
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Scroller.h" // Bring in our new blueprint

const int NUM_MENU_ITEMS = 3;
String menuItems[NUM_MENU_ITEMS] = {
  "1. WiFi Setup",
  "2. Settings",
  "3. Device Info"
};
int menuIndex = 0; 
String lastDirection = "";
bool inSubMenu = false;

// Create a scroller object using our blueprint! 
// We give it the text, row 1 (the bottom row), and a speed of 300ms.
TextScroller deviceInfoScroller("Arduino Uno R4 WiFi is connected and ready!", 1, 300);

void drawMainMenu(LiquidCrystal_I2C &lcd) {
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print("Main Menu:");
  lcd.setCursor(0, 1);
  lcd.print(menuItems[menuIndex]); 
}

void executeSelection(LiquidCrystal_I2C &lcd) {
  lcd.clear();
  lcd.setCursor(0, 0);
  
  if (menuIndex == 0) {
    lcd.print("Scanning WiFi...");
    lcd.setCursor(0, 1);
    lcd.print("< LEFT to go back");
  } else if (menuIndex == 1) {
    lcd.print("Settings Screen");
    lcd.setCursor(0, 1);
    lcd.print("< LEFT to go back");
  } else if (menuIndex == 2) {
    lcd.print("Device Info:");
    // Reset the scroller so it starts from the beginning every time we enter this screen
    deviceInfoScroller.reset(); 
  }
}

void handleMenu(String currentDirection, LiquidCrystal_I2C &lcd) {
  
  if (currentDirection != lastDirection && currentDirection != "CENTER") {
    
    if (inSubMenu == false) {
      if (currentDirection == "DOWN" && menuIndex < (NUM_MENU_ITEMS - 1)) {
        menuIndex++;
        drawMainMenu(lcd);
      } 
      else if (currentDirection == "UP" && menuIndex > 0) {
        menuIndex--;
        drawMainMenu(lcd);
      }
      else if (currentDirection == "PRESSED") {
        inSubMenu = true;          
        executeSelection(lcd);     
      }
    } 
    else {
      if (currentDirection == "LEFT") {
        inSubMenu = false;         
        drawMainMenu(lcd);         
      }
    }
    lastDirection = currentDirection;
  }

  // If we are in the Device Info screen, tell the scroller to do its job
  if (inSubMenu == true && menuIndex == 2) {
    deviceInfoScroller.update(lcd);
  }
}