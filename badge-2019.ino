///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                               //
//      ,---,.                                                                    ,----,     ,----..        ,---,  '   ,'  '.    //
//    ,'  .'  \             ,--,         ,---,                                  .'   .' \   /   /   \    ,`--.' | /   /      \   //
//  ,---.' .' |           ,--.'|       ,---.'|                                ,----,'    | /   .     :  /    /  :.   ;  ,/.  :   //
//  |   |  |: | .--.--.   |  |,        |   | :            .--.--.             |    :  .  ;.   /   ;.  \:    |.' ''   |  | :  ;   //
//  :   :  :  //  /    '  `--'_        |   | |   ,---.   /  /    '            ;    |.'  /.   ;   /  ` ;`----':  |'   |  ./   :   //
//  :   |    ;|  :  /`./  ,' ,'|     ,--.__| |  /     \ |  :  /`./            `----'/  ; ;   |  ; \ ; |   '   ' ;|   :       ,   //
//  |   :     \  :  ;_    '  | |    /   ,'   | /    /  ||  :  ;_                /  ;  /  |   :  | ; | '   |   | | \   \      |   //
//  |   |   . |\  \    `. |  | :   .   '  /  |.    ' / | \  \    `.            ;  /  /-, .   |  ' ' ' :   '   : ;  `---`---  ;   //
//  '   :  '; | `----.   \'  : |__ '   ; |:  |'   ;   /|  `----.   \          /  /  /.`| '   ;  \; /  |   |   | '     |   |  |   //
//  |   |  | ; /  /`--'  /|  | '.'||   | '/  ''   |  / | /  /`--'  /        ./__;      :  \   \  ',  /    '   : |     '   :  ;   //
//  |   :   / '--'.     / ;  :    ;|   :    :||   :    |'--'.     /         |   :    .'    ;   :    /     ;   |.'     |   |  '   //
//  |   | ,'    `--'---'  |  ,   /  \   \  /   \   \  /   `--'---'          ;   | .'        \   \ .'      '---'       ;   |.'    //
//  `----'                 ---`-'    `----'     `----'                      `---'            `---`                    '---'      //
//                                                                                                                               //
//                                                                                                                               //
//                                                                                                                               //
//                                                                                                                               //
//                                                                 
//                                                                                                                             
// Bsides Badge 2019
//  ( ascii art: http://patorjk.com/software/taag )
//                                                                                                                     
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

// Wifi password input via "secrets.h" (to avoid committing to git).
// Not specifying password will keep the same one set as before upload.
#include "secrets.h"
/* To change wifi password create a new tab "secrets.h" and copy these two lines into it:
#define SECRETS_SSID "ssid"
#define SECRETS_PASSWORD "password"
*/

#include <WiFi.h>  // ESP32 inbuilt library
#include <SPI.h>  // For TFT, ESP32 inbuilt library
#include <WiFiUdp.h> // For NTPClient, inbuilt libray
#include <HTTPClient.h>  //ESP32 inbuilt, for web requests (get sessions list)

#define FS_NO_GLOBALS
#include <FS.h>

#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

// Libraries from Arduino Library manager (links to help point to correct one)
#include <TaskScheduler.h>  // https://github.com/arkhipenko/TaskScheduler
#include <NTPClient.h>  // https://github.com/arduino-libraries/NTPClient
#include <FastLED.h>  // https://github.com/FastLED/FastLED
#include <JPEGDecoder.h>  // https://github.com/Bodmer/JPEGDecoder

#include "TFT_eSPI/TFT_eSPI.h"   // Local copy of TFT_eSPI: https://github.com/Bodmer/TFT_eSPI
#include "timelib.h"

// TFT Screen
TFT_eSPI tft = TFT_eSPI();  // Create screen object 240x240
TFT_eSprite img = TFT_eSprite(&tft);  // Sprite for in memory rendering

// Buttons
const int pinSw1 = 0;  //Up
//const int pinSw2 = 0; // ESP_EN -- reset pin on back
const int pinSw3 = 34; //Right
const int pinSw4 = 36; //Down
const int pinSw5 = 35; //Left
const int pinSw6 = 2;  //Top
const int pinSw7 = 39; //Mid
const int pinSw8 = 33; //Bottom

// Charging indication
const int pinChargeInd = 32;

// Output pins
const int pinOutR = 22;
const int pinOutG = 21;
const int pinOutB = 19;
const int pinOutH = 18;
const int pinOutV = 5;

// Addressable LEDs (APA102s)
const int pinLedsData = 14;
const int pinLedsClock = 12;

// Task Scheduler
Scheduler runner;
// - prototypes for tasks
void runWifiCheck();
void runTimeSync();
void runDisplayTime();
void idleDisplay();
bool idleOnEnable();
void imagesDisplay();
void updateSessionsList();
void checkLoop();
bool checkSetup();
bool enableSessionsDisplay();
void loopSessionsDisplay();
// - tasks themselves 
//     Task tTask(update time ms, update count, address of function);
Task tWifiCheck(  5 * 1000, TASK_FOREVER, &runWifiCheck);
Task tTimeSync( 60 * 1000, TASK_FOREVER, &runTimeSync);
Task tDisplayTime( 1 * 1000, TASK_FOREVER, &runDisplayTime);
Task tIdleDisplay( 10, TASK_FOREVER, &idleDisplay, NULL, false, &idleOnEnable);
Task tImagesDisplay( 3000, TASK_FOREVER, &imagesDisplay);
Task tGetSessionsList(500, 1, &updateSessionsList);
Task tSessionsDisplay(500, TASK_FOREVER, &loopSessionsDisplay, NULL, false, &enableSessionsDisplay);
Task tCheckLoop(1000, TASK_FOREVER, &checkLoop, NULL, false, &checkSetup);

// Prototypes for setup
void setupWifi();
void readSessionListFromFlash();

void setup(void) {
  // Start and clear screen 
  // (helps reduce the garbage pattern on TFT at poweron)
  tft.init();
  tft.fillScreen(TFT_BLACK);
  
  //Serial.begin(115200);
  Serial.begin(2000000);
  Serial.println("Bsides Badge 2019 starting");

  setupButtons(); 

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }

  //imagesSetup();
  //setupSpriteTest();

  // Start wifi
  setupWifi();
  readSessionListFromFlash();

  // Start scheduler
  runner.init();
  // - add tasks
  runner.addTask(tWifiCheck);
  runner.addTask(tTimeSync);
  runner.addTask(tDisplayTime);
  runner.addTask(tIdleDisplay);
  runner.addTask(tImagesDisplay);
  runner.addTask(tGetSessionsList);
  runner.addTask(tSessionsDisplay);
  runner.addTask(tCheckLoop);
  // - start tasks
  tWifiCheck.enable();
  tTimeSync.enable();
  //tDisplayTime.enable();
  //tIdleDisplay.enable();
  //tImagesDisplay.enable();
  //tImagesDisplay.forceNextIteration();
  //tCheckLoop.enable();
  tSessionsDisplay.enable();
  Serial.println("Initialised scheduler");

  // Print badges unique ID (mac address)
  uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.
  
}

void loop() {
  runner.execute();
  //loopSpriteTest();
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

void setupButtons() {
  pinMode(pinSw1, INPUT);
  pinMode(pinSw3, INPUT);
  pinMode(pinSw4, INPUT);
  pinMode(pinSw5, INPUT);
  pinMode(pinSw6, INPUT);
  pinMode(pinSw7, INPUT);
  pinMode(pinSw8, INPUT);
}

// Task cycle LEDs

// Task display current/next session

// Maintain wifi

// Download bsides details

// Service infrared
