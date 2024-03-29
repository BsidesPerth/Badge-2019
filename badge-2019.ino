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
//#include "secrets.h"
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
#define FASTLED_INTERNAL // to suppress pragma messages during build
#include <FastLED.h>  // https://github.com/FastLED/FastLED
#include <JPEGDecoder.h>  // https://github.com/Bodmer/JPEGDecoder
#include <Bounce2.h>  //https://github.com/thomasfredericks/Bounce2
#include <ArduinoJson.h>  //https://arduinojson.org/

#include "TFT_eSPI/TFT_eSPI.h"   // Local copy of TFT_eSPI: https://github.com/Bodmer/TFT_eSPI
#include "timelib.h"  // local copy since ESP32 seems to miss <time.h>

// TFT Screen
#include "Free_Fonts.h" // Helpers for the TFT eSPI adafruit "free fonts"
TFT_eSPI tft = TFT_eSPI();  // Create screen object 240x240
TFT_eSprite img = TFT_eSprite(&tft);  // Sprite for in memory rendering

// TFT LCD Backlight
const int pinLcdBacklight = 23;

// Buttons
#define DEBOUNCE_INTERVAL 25
#define NUMBER_BUTTONS 7
enum EBUTTONS {
  BUTTON_UP,
  BUTTON_RIGHT,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_TOP,
  BUTTON_MID,
  BUTTON_BOTTOM
};

struct buttons_t {
  const int pin;  // pin number
  const int mode; // INPUT, INPUT_PULLUP, etc
  const bool offState; // logic level when not pressed
  Bounce debouncer;  // manages debouncing logic
  void (*callback)(EBUTTONS button, bool pressed);  // called when button pressed or released
};

buttons_t buttons[NUMBER_BUTTONS] = {
  {0,  INPUT, HIGH, Bounce(), nullptr},  // SW2 = BUTTON_UP
  {34, INPUT, HIGH, Bounce(), nullptr},  // SW3 = BUTTON_RIGHT
  {36, INPUT, HIGH, Bounce(), nullptr},  // SW4 = BUTTON_DOWN
  {35, INPUT, HIGH, Bounce(), nullptr},  // SW5 = BUTTON_LEFT
  {18, INPUT_PULLUP, HIGH, Bounce(), nullptr},  // SW6 = BUTTON_TOP
  {39, INPUT, HIGH, Bounce(), nullptr},  // SW7 = BUTTON_MID
  {33, INPUT, HIGH, Bounce(), nullptr}   // SW8 = BUTTON_BOTTOM
};


// Charging indication
const int pinChargeInd = 32;

// Output pins
const int pinOutR = 22;
const int pinOutG = 21;
const int pinOutB = 19;
const int pinOutS = 25;
const int pinOutH = 12;
const int pinOutV = 5;

// IR
const int pinIRtx = 13;
const int pinIRrx = 15;

// Addressable LEDs (APA102s)
const int pinLedsData = 14;
const int pinLedsClock = 2;

// FastLED
#define NUM_LEDS    8
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
#define BRIGHTNESS  10
//#define BRIGHTNESS  32
CRGB leds[NUM_LEDS];
bool ledRainbow = true;


// Task Scheduler
Scheduler runner;
// - prototypes for tasks
void runWifiCheck();
void runTimeSync();
void runDisplayTime();
void nameTagLoop();
bool nameTagEnable();
void nameTagDisable();
void nameEditLoop();
bool nameEditEnable();
void nameEditDisable();
bool imagesEnable();
void imagesDisplay();
void updateSessionsList();
void checkLoop();
bool checkEnable();
void checkDisable();
bool enableSessionsDisplay();
void loopSessionsDisplay();
void updateButtons();
void updateFastLED();
void ledRainbowCycle();
bool menuEnable();
void menuLoop();
void checkRAM();
bool badgeemEnable();
void badgeemDisable();
void badgeemLoop();
void badgeemRegisterLoop();
void gameoflifeLoop();
bool gameoflifeEnable();
void gameoflifeDisable();
// - tasks themselves 
//     Task tTask(update time ms, update count, address of function);
Task tWifiCheck(  5 * 1000, TASK_FOREVER, &runWifiCheck);
Task tTimeSync( 60 * 1000, TASK_FOREVER, &runTimeSync);
Task tDisplayTime( 1 * 1000, TASK_FOREVER, &runDisplayTime);
Task tNameTag( 100, TASK_FOREVER, &nameTagLoop, NULL, false, &nameTagEnable, &nameTagDisable);
Task tNameEdit( 10000, TASK_FOREVER, NULL, NULL, false, &nameEditEnable, &nameEditDisable);
Task tImagesDisplay( 3000, TASK_FOREVER, &imagesDisplay, NULL, false, &imagesEnable);
Task tGetSessionsList(500, 1, &updateSessionsList);
Task tSessionsDisplay(1000, TASK_FOREVER, &loopSessionsDisplay, NULL, false, &enableSessionsDisplay);
Task tCheckLoop(1000, TASK_FOREVER, &checkLoop, NULL, false, &checkEnable, &checkDisable);
Task tUpdateButtons(1, TASK_FOREVER, &updateButtons);
Task tFastLED(10, TASK_FOREVER, &updateFastLED);
Task tLedRainbow(100, TASK_FOREVER, &ledRainbowCycle);
Task tMenu(100, TASK_FOREVER, &menuLoop, NULL, false, &menuEnable);
Task tCheckRAM(30000, TASK_FOREVER, &checkRAM);
Task tBadgeem(1000, TASK_FOREVER, &badgeemLoop, NULL, false, &badgeemEnable, &badgeemDisable);
Task tBadgeemRegister(60 * 1000, TASK_FOREVER, &badgeemRegisterLoop);
Task tGameOfLife(1, TASK_FOREVER, &gameoflifeLoop, NULL, false, &gameoflifeEnable, &gameoflifeDisable);

// Prototypes for setup
void setupWifi();
void readSessionListFromFlash();
bool checkFirstStart();

void setup(void) {
  // Start and clear screen 
  // (helps reduce the garbage pattern on TFT at poweron)
  tft.init();
  tft.fillScreen(TFT_BLACK);
  
  // Turn on backlight
  pinMode(pinLcdBacklight, OUTPUT);
  digitalWrite(pinLcdBacklight, HIGH);

  Serial.begin(115200);
  //Serial.begin(2000000);
  Serial.println(F("Bsides Badge 2019 starting"));

  // FastLED - initialse and blank
  FastLED.addLeds<LED_TYPE,pinLedsData,pinLedsClock,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  //fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();

  // Initialise filesystem (onboard flash)
  if (!SPIFFS.begin()) {
    Serial.println(F("SPIFFS initialisation failed!"));
    leds[0] = CRGB::Yellow;
    leds[1] = CRGB::Yellow;
    FastLED.show();
    while (1) {
      yield(); // stay here - infinite loop
    }
  }

  // Initialse various modules
  setupButtons(); 
  setupWifi();
//  Serial.println(F("test point 1"));
//  Serial.flush();
//  listFiles();  // hogs memory
//  Serial.println(F("test point 2"));
//  Serial.flush();
  readSessionListFromFlash();
  readTagFromFlash();
  badgeemRegisterLoop();

  // Start scheduler
  runner.init();
  // - add tasks
  runner.addTask(tUpdateButtons);
  runner.addTask(tWifiCheck);
  runner.addTask(tTimeSync);
  runner.addTask(tDisplayTime);
  runner.addTask(tNameTag);
  runner.addTask(tNameEdit);
  runner.addTask(tImagesDisplay);
  runner.addTask(tGetSessionsList);
  runner.addTask(tSessionsDisplay);
  runner.addTask(tCheckLoop);
  runner.addTask(tFastLED);
  runner.addTask(tMenu);
  runner.addTask(tCheckRAM);
  runner.addTask(tLedRainbow);
  runner.addTask(tBadgeem);
  runner.addTask(tBadgeemRegister);
  runner.addTask(tGameOfLife);
  // - start background tasks
  tUpdateButtons.enable();
  tWifiCheck.enable();
  tTimeSync.enable();
  tFastLED.enable();
  tLedRainbow.enable();
  tCheckRAM.enable();
  //tBadgeemRegister.enable();
  // - start display task
  if (checkFirstStart()) {
    tCheckLoop.enable();
  } else {
    //tMenu.enable();
    tNameTag.enable();
  }
  Serial.println(F("Initialised scheduler"));

  // Print badges unique ID (mac address)
  uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
//  Serial.printf(F("ESP32 Chip ID = %04X"),(uint16_t)(chipid>>32));//print High 2 bytes
//  Serial.printf(F("%08X\n"),(uint32_t)chipid);//print Low 4bytes.
  Serial.print(F("ESP32 Chip ID = "));
  Serial.print((uint16_t)(chipid>>32), HEX);//print High 2 bytes
  Serial.println((uint32_t)chipid, HEX);//print Low 4bytes.
  
}

void loop() {
  runner.execute();
}

void setupButtons() {
  for (int i=0; i<NUMBER_BUTTONS; i++) {
    buttons[i].debouncer.attach(buttons[i].pin, buttons[i].mode);
    buttons[i].debouncer.interval(DEBOUNCE_INTERVAL);
  }
}

void updateButtons() {
  for (int i=0; i<NUMBER_BUTTONS; i++) {
    buttons[i].debouncer.update();

    if (buttons[i].callback) {
      // Fire the callback if we have a genuine button state change.
      // Debouncer filters the noise for us.
      if (buttons[i].debouncer.rose()) {
        // Went from low to high
        //  - if offState=LOW(false) then thats pressing it (pressed = true)
        //  - if offState=HIGH(true) then thats releasing it (pressed = false)
        buttons[i].callback(static_cast<EBUTTONS>(i), !buttons[i].offState);
      } else if (buttons[i].debouncer.fell()) {
        // Went from high to low
        //  - if offState=LOW(false) then thats releasing it (pressed = false)
        //  - if offState=HIGH(true) then thats pressing it (pressed = true)
        buttons[i].callback(static_cast<EBUTTONS>(i), buttons[i].offState);
      }
    }
  }
}

void ledRainbowCycle() {
  static int gHue = 0;
  gHue++;
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void updateFastLED() {
  FastLED.show();
}

CRGB tftColourToFastLED(unsigned int colour) {
  int red = ((colour >> 11) & 0b11111) * 8;
  int green = ((colour >> 5) & 0b111111) * 4;
  int blue = (colour & 0b11111) * 8;
  return CRGB(red, green, blue);
}

void checkRAM() {
  Serial.print(F("Check RAM: "));
  Serial.println(ESP.getFreeHeap());
}
