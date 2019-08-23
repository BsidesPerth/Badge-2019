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

#include <TaskScheduler.h>
#include <WiFi.h>
#include <SPI.h>  // For TFT
#include "TFT_eSPI/TFT_eSPI.h"       // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library



void setup(void) {
  Serial.begin(115200);
  Serial.println("Bsides Badge 2019 starting");

  // Delay to notice restarts and bootloops
  delay(1000);

  tft.init();

  demoScreen();

  setupWifi();

  delay(5000);

  uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.
  
}

void loop() {
  checkWifi();
}

void demoScreen() {
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

}

// Task cycle LEDs

// Task display current/next speaker

// Maintain wifi

// Download bsides details

// Service infrared
