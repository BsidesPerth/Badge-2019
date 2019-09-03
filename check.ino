// Check the components on the board

bool checkButtonsToggle[NUMBER_BUTTONS] = {0};

bool checkSetup() {
  Serial.println("Test Setup");
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawCentreString("Self-Check", 120, 0, 4);

  for (int i=0; i<NUMBER_BUTTONS; i++) {
    checkButtonsToggle[i] = false;
    buttons[i].callback = checkButtonHandler;
  }  

  return true;
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
  Serial.print("Self-Check: ");
  
  const int vertSpace = 30;
  for (int i=0; i<7; i++) {
    // No switch 2
    int number = (i>0) ? i+2 : i+1;
    int pinStatus = buttons[i].debouncer.read();
    drawButton(number, pinStatus, checkButtonsToggle[i], 10, 30 + vertSpace*i);
    Serial.printf("%d, ", pinStatus);
  }
  Serial.println();
}

void drawButton(int number, bool status, bool toggle, int x, int y) {
  img.createSprite(50, 20);
  int backCol = status ? (toggle ? TFT_ORANGE : TFT_RED) : TFT_GREEN;
  int foreCol = status ? TFT_WHITE : TFT_BLACK;
  img.fillSprite(backCol);
  img.setTextColor(foreCol, foreCol);
  String text = String("SW") + String(number) + String(": ") + String(status);
  img.drawString(text, 5, 5, 2);
  img.pushSprite(x, y);
  img.deleteSprite();
}
