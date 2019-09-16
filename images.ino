// Images to be displayed
// Must use "Arduino -> Tools -> ESP Sketch Data Upload" to put images on device
//
// NOTE: ESP32 JPEG library will report
//  "Jpeg file format not supported!"
// if JPEG is saved with "progressive" (default in most programs).
// Use GIMP and untick "progressive" when JPEG dialog pops up.
//
// TODO: Add bonus crazy parot emoji video

void listFiles();
void drawJpeg(const char *filename, int xpos, int ypos);

bool imagesEnable() {
  // Just do first image
  //imagesDisplay();
  return true;
}

void imagesDisplay() {
  
  static int counter = 0;
  const int items = 6;
  if (counter % items == 0) {
    //tft.fillScreen(TFT_WHITE);
    drawJpeg("/bird_b.jpg", 0, 0);
  } else if (counter % items == 1) {
    //tft.fillScreen(TFT_WHITE);
    drawJpeg("/bronze1_b.jpg", 0, 0);
  } else if (counter % items == 2) {
    //tft.fillScreen(TFT_BLACK);
    drawJpeg("/bronze2_b.jpg", 0, 0);
  } else if (counter % items == 3) {
    //tft.fillScreen(TFT_BLACK);
    drawJpeg("/gold-kit_b.jpg", 0, 0);
  } else if (counter % items == 4) {
    //tft.fillScreen(TFT_BLACK);
    drawJpeg("/venue_b.jpg", 0, 0);
  } else {
    // Switch over to idle
    counter = 0;
    tImagesDisplay.disable();
    tMenu.enable();
  }
  counter += 1;
}
