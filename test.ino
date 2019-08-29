// Test the components on the board

TFT_eSprite testImg = TFT_eSprite(&tft);

bool testSetup() {
  Serial.println("Test Setup");
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextFont(3);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawCentreString("Board Test", 100, 0, 3);

  testImg.createSprite(200, 200);
  testImg.fillSprite(TFT_BLACK);

  return true;
}

void testLoop() {
  Serial.println("Test Loop");
  testImg.fillSprite(TFT_BLACK);
  testImg.setTextSize(1);
  testImg.setTextFont(3);

  int pinStatus = digitalRead(pinSw1);
  int colour = pinStatus ? TFT_RED : TFT_GREEN;
  testImg.setTextColor(TFT_WHITE, colour);
  String text = String("SW") + String(1) + String(": ") + String(pinStatus);
  testImg.drawString(text, 0, 20, 3);

  //testImg.pushSprite(20, 20);
}
