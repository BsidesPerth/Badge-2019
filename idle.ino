// Idle screen shown when badge not in active use
// Can set badge name

// Some details on fonts:
// * setTextFont() 1 to 8
//   * 1 = GLCD font (tiny) "Standard ASCII 5x7 font"
//   * 2 = Font 16: 16pt, normal font, 96 chars
//   * 3 = unused
//   * 4 = Font 32: 26pt, normal font, 96 chars
//   * 5 = unused
//   * 6 = Font 64: 48pt Numbers and Time Only! "[space] 0 1 2 3 4 5 6 7 8 9 : - . a p m"
//   * 7 = 7 Segment LCD Font (48pt) Numbers Only! "[space] 0 1 2 3 4 5 6 7 8 9 : . -"
//   * 8 = Font 72 (Arial 75pt) Numbers Only! "[space] 0 1 2 3 4 5 6 7 8 9 0 : - ."
// * setTextSize() is scaling multiplier on font
// * Adafruit free fonts available (better range of heights) but might be slower, more flickery?



bool idleOnEnable() {
  tft.fillScreen(TFT_BLACK);

  // Button handler
  for (int i=0; i<NUMBER_BUTTONS; i++) {
    buttons[i].callback = menuButtonHandler;
  }  

  // White text
  // Do not plot the background colour by setting to same colour
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  //img.setTextColor(TFT_YELLOW, TFT_YELLOW);

  tft.setTextSize(3);
  tft.drawCentreString("Bsides", 120, 50, 4);
  tft.setTextSize(1);
  tft.drawCentreString("2019", 120, 130, 8);

  return true;
}

void menuButtonHandler(EBUTTONS button, bool pressed) {
  if (pressed) {
    // On any button, return to menu
    tIdleDisplay.disable();
    tMenu.enable();
  }
}

void idleDisplay() {
//  // Background rainbow
//  const byte rainbowSaturation = 255; // 200
//  const byte rainbowIntensity = 250;
//  static int loopCounter = 0;
//  for (int i = 0; i < img.height(); i++) {
//    CRGB col = CHSV(i + loopCounter, rainbowSaturation, rainbowIntensity);
//    unsigned int colour = col.red/8 << 11 | col.green/4 << 5 | col.blue/8;
//    img.drawFastHLine(0, i, img.width(), colour);
//  }
//  loopCounter += 1;
//
//  // White text
//  // Do not plot the background colour by setting to same colour
//  img.setTextColor(TFT_WHITE, TFT_WHITE);
//  //img.setTextColor(TFT_YELLOW, TFT_YELLOW);
//  
//
//  img.setTextSize(3);
//  img.drawCentreString("Bsides", 115, 50, 4);
//  img.setTextSize(1);
//  img.drawCentreString("2019", 115, 130, 8);
//
//  // Draw to screen
//  img.pushSprite(5, 5);
}
