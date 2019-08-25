void listFiles();
void drawJpeg(const char *filename, int xpos, int ypos);

void imagesSetup() {
  tft.fillScreen(TFT_BLACK);
  listFiles(); // Lists the files so you can see what is in the SPIFFS
}

void imagesDisplay() {
  
  static int counter = 0;
  const int items = 4;
  if (counter % items == 0) {
    tft.fillScreen(TFT_WHITE);
    drawJpeg("/logo_240.jpg", 0, 73);
  } else if (counter % items == 1) {
    tft.fillScreen(TFT_WHITE);
    drawJpeg("/sponsors.jpg", 56, 56);
  } else if (counter % items == 2) {
    tft.fillScreen(TFT_BLACK);
    drawJpeg("/boops.jpg", 56, 56);
  } else {
    // Switch over to idle
    tImagesDisplay.disable();
    tIdleDisplay.enable();
  }
  counter += 1;
}
