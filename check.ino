// Check the components on the board
// TODO: Add some way to exit
// TODO: Missing other devices?

bool checkButtonsToggle[NUMBER_BUTTONS] = {0};

bool checkEnable() {
  Serial.println(F("Test Setup"));
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawCentreString(F("Self-Check"), 120, 0, 4);

  for (int i=0; i<NUMBER_BUTTONS; i++) {
    checkButtonsToggle[i] = false;
    buttons[i].callback = checkButtonHandler;
  }  

  tLedRainbow.disable();

  return true;
}

void checkDisable() {
  tLedRainbow.enable();
}

void checkButtonHandler(EBUTTONS button, bool pressed) {
  if (pressed) {
    // Toggle
    checkButtonsToggle[button] = !checkButtonsToggle[button];
  }
  
  // Event driven check update
  checkLoop();
}

void checkLoop() {
  Serial.print(F("Self-Check: "));
  
  const int vertSpace = 30;
  for (int i=0; i<7; i++) {
    // No switch 2
    int number = (i>0) ? i+2 : i+1;
    int pinStatus = buttons[i].debouncer.read();
    bool toggleStatus = checkButtonsToggle[i];
    
    String text = String("SW") + String(number) + String(": ") + String(toggleStatus);
    drawButton(text, pinStatus, toggleStatus, 10, 30 + vertSpace*i);
    Serial.printf("%d, ", pinStatus);

    if (i < 6) {
      // Do fastLED colours
      int colour = pinStatus ? (toggleStatus ? TFT_BLUE : TFT_RED) : TFT_GREEN;
      String text = String("LED") + String(number);
      drawButton(text, pinStatus, checkButtonsToggle[i], 80, 30 + vertSpace*i);
      leds[i] = tftColourToFastLED(colour);
      if (i >= 4) {
        // Double duty to cover all LEDs
        leds[i+2] = tftColourToFastLED(colour);
      }
    }

    if (i == 6) {
      // SW1 toggles IR
      int val = checkButtonsToggle[i];
      digitalWrite(pinIRtx, val);

      drawButton("IR TX", val, false, 80, 30 + vertSpace*i);
    }
  }
  Serial.println();
  FastLED.show();
}

//void drawIndicator(String text, bool status, int x, int y) {
//  img.createSprite(50, 20);
//  int backCol = status ? TFT_RED : TFT_GREEN;
//  int foreCol = status ? TFT_WHITE : TFT_BLACK;
//  img.fillSprite(backCol);
//  img.setTextColor(foreCol, foreCol);
//  img.drawString(text, 5, 5, 2);
//  img.pushSprite(x, y);
//  img.deleteSprite();
//}

void drawButton(const String & text, bool status, bool toggle, int x, int y) {
  img.createSprite(50, 20);
  int backCol = status ? (toggle ? TFT_BLUE : TFT_RED) : TFT_GREEN;
  int foreCol = status ? TFT_WHITE : TFT_BLACK;
  img.fillSprite(backCol);
  img.setTextColor(foreCol, foreCol);
  img.drawString(text, 5, 5, 2);
  img.pushSprite(x, y);
  img.deleteSprite();
}
