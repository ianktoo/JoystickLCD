// --- Scroller.h ---
#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class TextScroller {
  private:
    String fullText;
    unsigned long lastScrollTime;
    int scrollPosition;
    int scrollSpeed; // Time in milliseconds between shifts
    int displayRow;  // Which row of the LCD to print on (0 or 1)

  public:
    // This is the "Constructor". It sets up the object when you first create it.
    TextScroller(String textToScroll, int row, int speed) {
      // Automatically pad the text with 16 spaces so it loops smoothly
      fullText = textToScroll + "                "; 
      displayRow = row;
      scrollSpeed = speed;
      scrollPosition = 0;
      lastScrollTime = 0;
    }

    // Call this whenever you open the menu screen to start from the beginning
    void reset() {
      scrollPosition = 0;
      lastScrollTime = millis();
    }

    // Call this continuously in your loop
    void update(LiquidCrystal_I2C &lcd) {
      if (millis() - lastScrollTime > scrollSpeed) {
        lastScrollTime = millis();
        
        lcd.setCursor(0, displayRow);
        
        // Grab the 16-character chunk
        String displayChunk = fullText.substring(scrollPosition, scrollPosition + 16);
        lcd.print(displayChunk);
        
        scrollPosition++;
        
        // Reset to the beginning if we reach the end
        if (scrollPosition > fullText.length() - 16) {
          scrollPosition = 0;
        }
      }
    }
};