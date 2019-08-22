// Bsides Badge 2019
//
// __ Arduino IDE setup __
// Add this in Preferences -> Additional Boards URLs:
//  https://dl.espressif.com/dl/package_esp32_index.json
// Boards setting:
// * WEMOS LOLIN32
// * Upload Speed: 921600 (default)
// * Flash Frequency 80MHz (default)
// * Partition Scheme: Default

// === Just a placeholder TFT demo file follows ===

//   Diagnostic test for the displayed colour order
//
// Writen by Bodmer 17/2/19 for the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI
#include <SPI.h>
#include "TFT_eSPI/TFT_eSPI.h"       // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

void setup(void) {
  tft.init();

  tft.fillScreen(TFT_BLACK);
  
  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(0, 0, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // We can now plot text on screen using the "print" class
  tft.println("Bsides 2019\n");
  tft.println("White text");
  
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.println("Red text");
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Green text");
  
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.println("Blue text");

  delay(5000);

}

void loop() {}
