//
//
//
//      ,---,.                                                                    ,----,     ,----..        ,---,  '   ,'  '.  
//    ,'  .'  \             ,--,         ,---,                                  .'   .' \   /   /   \    ,`--.' | /   /      \ 
//  ,---.' .' |           ,--.'|       ,---.'|                                ,----,'    | /   .     :  /    /  :.   ;  ,/.  : 
//  |   |  |: | .--.--.   |  |,        |   | :            .--.--.             |    :  .  ;.   /   ;.  \:    |.' ''   |  | :  ; 
//  :   :  :  //  /    '  `--'_        |   | |   ,---.   /  /    '            ;    |.'  /.   ;   /  ` ;`----':  |'   |  ./   : 
//  :   |    ;|  :  /`./  ,' ,'|     ,--.__| |  /     \ |  :  /`./            `----'/  ; ;   |  ; \ ; |   '   ' ;|   :       , 
//  |   :     \  :  ;_    '  | |    /   ,'   | /    /  ||  :  ;_                /  ;  /  |   :  | ; | '   |   | | \   \      | 
//  |   |   . |\  \    `. |  | :   .   '  /  |.    ' / | \  \    `.            ;  /  /-, .   |  ' ' ' :   '   : ;  `---`---  ; 
//  '   :  '; | `----.   \'  : |__ '   ; |:  |'   ;   /|  `----.   \          /  /  /.`| '   ;  \; /  |   |   | '     |   |  | 
//  |   |  | ; /  /`--'  /|  | '.'||   | '/  ''   |  / | /  /`--'  /        ./__;      :  \   \  ',  /    '   : |     '   :  ; 
//  |   :   / '--'.     / ;  :    ;|   :    :||   :    |'--'.     /         |   :    .'    ;   :    /     ;   |.'     |   |  ' 
//  |   | ,'    `--'---'  |  ,   /  \   \  /   \   \  /   `--'---'          ;   | .'        \   \ .'      '---'       ;   |.'  
//  `----'                 ---`-'    `----'     `----'                      `---'            `---`                    '---'    
//                                                                                                                             
//
//                                                                 
//                                                                                                                             
// Bsides Badge 2019
//  ( ascii art: http://patorjk.com/software/taag )
//                                                                                                                  ,---.-,    
// __ Arduino IDE setup __
// Add this in Preferences -> Additional Boards URLs:
//  https://dl.espressif.com/dl/package_esp32_index.json
//
// Board settings:
// * Board: ESP32 Dev Module
// * Upload Speed: 921600 (default)
// * CPU Frequency: 240MHz (default)
// * Flash Frequency: 80MHz (default)
// * Flash Mode: QIO (default)
// * Flash Size: 4MB (32Mb) (default)
// * Partition Scheme: Default 4MB with spiffs (default)
// * Core debug level: None -> Verbose (extra logging over serial)
// * PSRAM: Disabled (default)
//
// Alternate board settings:
// * Board: WEMOS LOLIN32
// * Upload Speed: 921600 (default)
// * Flash Frequency: 80MHz (default)
// * Partition Scheme: Default
//

#include <WiFi.h>  // ESP32 inbuilt library
#include <SPI.h>  // For TFT, ESP32 inbuilt library
#include <WiFiUdp.h> // For NTPClient, inbuilt libray

#define FS_NO_GLOBALS
#include <FS.h>

#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

#include <TaskScheduler.h>  // https://github.com/arkhipenko/TaskScheduler
#include <NTPClient.h>  // https://github.com/arduino-libraries/NTPClient
#include <FastLED.h>  // https://github.com/FastLED/FastLED
#include <JPEGDecoder.h>  // https://github.com/Bodmer/JPEGDecoder

#include "TFT_eSPI/TFT_eSPI.h"   // Local copy of TFT_eSPI: https://github.com/Bodmer/TFT_eSPI

// TFT Screen
TFT_eSPI tft = TFT_eSPI();  // Create screen object 240x240

// Task Scheduler
Scheduler runner;
// - prototypes for tasks
void runWifiCheck();
void runTimeSync();
void runDisplayTime();
void idleDisplay();
bool idleOnEnable();
void imagesDisplay();
void sendGET();
// - tasks themselves 
//     Task tTask(update time ms, update count, address of function);
Task tWifiCheck(  5 * 1000, TASK_FOREVER, &runWifiCheck);
Task tTimeSync( 60 * 1000, TASK_FOREVER, &runTimeSync);
Task tDisplayTime( 1 * 1000, TASK_FOREVER, &runDisplayTime);
Task tIdleDisplay( 10, TASK_FOREVER, &idleDisplay, NULL, false, &idleOnEnable);
Task tImagesDisplay( 3000, TASK_FOREVER, &imagesDisplay);
Task tsendGET(1000, 1, &sendGET);

void setupWifi();

void setup(void) {
  Serial.begin(115200);
  Serial.println("Bsides Badge 2019 starting");

  tft.init();

  //demoScreen();
  
  idleSetup();
  imagesSetup();

  // Start wifi
  setupWifi();

  // Start scheduler
  runner.init();
  // - add tasks
  runner.addTask(tWifiCheck);
  runner.addTask(tTimeSync);
  runner.addTask(tDisplayTime);
  runner.addTask(tIdleDisplay);
  runner.addTask(tImagesDisplay);
  runner.addTask(tsendGET);
  // - start tasks
  tWifiCheck.enable();
  tTimeSync.enable();
  //tDisplayTime.enable();
  //tIdleDisplay.enable();
  tImagesDisplay.enable();
  tImagesDisplay.forceNextIteration();
  Serial.println("Initialised scheduler");

  // Print badges unique ID (mac address)
  uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.
  
}

void loop() {
  runner.execute();
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
