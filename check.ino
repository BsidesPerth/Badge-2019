// Check the components on the board
//
// After pressing a button, check will exit after 5 seconds of inactivity
//
// TODO: Missing other devices?
// TODO: Do IR pulse and read IR sensor

bool checkButtonsToggle[NUMBER_BUTTONS] = {0};
int checkCountdown = -1;
const int checkCountdownResetValue = 5;
bool checkStopping = false;

bool checkEnable() {
  tLedRainbow.disable();
  checkStopping = false;
  checkCountdown = -1;

  Serial.println(F("Test Setup"));
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawCentreString(F("Self-Check"), 120, 0, 4);

  for (int i=0; i<NUMBER_BUTTONS; i++) {
    checkButtonsToggle[i] = false;
    buttons[i].callback = checkButtonHandler;
  }  

  return true;
}

void checkDisable() {
  checkStopping = true;
  tLedRainbow.enable();
}

void checkButtonHandler(EBUTTONS button, bool pressed) {
  if (pressed) {
    // Toggle
    checkButtonsToggle[button] = !checkButtonsToggle[button];
  }
  
  // Event driven check update
  checkLoop();

  // Reset counter
  checkCountdown = checkCountdownResetValue;
}

void checkLoop() {
  if (checkStopping) {
    Serial.println(F("Check still stopping"));
    return;
  }
  
  Serial.print(F("Self-Check: "));

  if (checkCountdown == 0) {
    // exit
    Serial.println(F("Check countdown expire. Return to menu"));
    tCheckLoop.disable();
    tMenu.enable();
    return;
  }

  if (checkCountdown > 0) {
    checkCountdown--;
  }
  
  const int vertSpace = 30;
  for (int i=0; i<7; i++) {
    // No switch 1
    int number = i+2;
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
