// Check the components on the board

bool checkSetup() {
  Serial.println("Test Setup");
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawCentreString("Self-Check", 120, 0, 4);

  return true;
}

void checkLoop() {
  Serial.print("Self-Check: ");
  
  bool pinStatus[7] = {
    digitalRead(pinSw1),
    digitalRead(pinSw3),
    digitalRead(pinSw4),
    digitalRead(pinSw5),
    digitalRead(pinSw6),
    digitalRead(pinSw7),
    digitalRead(pinSw8),
  };

  const int vertSpace = 30;
  for (int i=0; i<7; i++) {
    // No switch 2
    int number = (i>0) ? i+2 : i+1;
    drawButton(number, pinStatus[i], 10, 30 + vertSpace*i);
    Serial.printf("%d, ", pinStatus[i]);
  }
  Serial.println();
}

void drawButton(int number, bool status, int x, int y) {
  img.createSprite(50, 20);
  int backCol = status ? TFT_RED : TFT_GREEN;
  int foreCol = status ? TFT_WHITE : TFT_BLACK;
  img.fillSprite(backCol);
  img.setTextColor(foreCol, foreCol);
  String text = String("SW") + String(number) + String(": ") + String(status);
  img.drawString(text, 5, 5, 2);
  img.pushSprite(x, y);
  img.deleteSprite();
}
