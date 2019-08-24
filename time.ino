// NTP time sync and time display

// NTP client uses a UDP connection
WiFiUDP ntpUDP;

// Perth timezone offset (+8 hours) in seconds
const int timeOffset = 28800;

// Connect to Australian NTP time server with Perth timezone
NTPClient timeClient(ntpUDP, "au.pool.ntp.org", timeOffset);


bool timeServerSetup = false;

void runTimeSync() {
  // Only start or update time server while connected to wifi
  if (WiFi.status() == WL_CONNECTED) {
    if (!timeServerSetup) {
      Serial.print("{TIME} Starting time server... ");
      timeClient.begin();
      Serial.println("DONE");
      Serial.println("{TIME} Updating initial time");
      timeServerSetup = true;
    } 
    
    if (timeServerSetup) {
      // Update time with NTP
      // Update at our own rate specified by how often task runs
      timeClient.forceUpdate();
      Serial.print("{TIME} Updated time: ");
      Serial.print(getDateStr());
      Serial.print(" ");
      Serial.print(getTimeStr());
      Serial.println();
    }
  }
}

String getTimeStr() {
  return timeClient.getFormattedTime();
}

// From: https://github.com/taranais/NTPClient/blob/master/NTPClient.cpp
#define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ) )
String getDateStr() {
  unsigned long rawTime = timeClient.getEpochTime() / 86400L;  // in days
  unsigned long days = 0, year = 1970;
  uint8_t month;
  static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

  while((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawTime)
    year++;
  rawTime -= days - (LEAP_YEAR(year) ? 366 : 365); // now it is days in this year, starting at 0
  days=0;
  for (month=0; month<12; month++) {
    uint8_t monthLength;
    if (month==1) { // february
      monthLength = LEAP_YEAR(year) ? 29 : 28;
    } else {
      monthLength = monthDays[month];
    }
    if (rawTime < monthLength) break;
    rawTime -= monthLength;
  }
  String monthStr = ++month < 10 ? "0" + String(month) : String(month); // jan is month 1  
  String dayStr = ++rawTime < 10 ? "0" + String(rawTime) : String(rawTime); // day of month  
  //return String(year) + "-" + monthStr + "-" + dayStr + "T" + this->getFormattedTime(secs ? secs : 0) + "Z";
  return String(year) + "-" + monthStr + "-" + dayStr;
}

void timeToScreen() {
  tft.fillScreen(TFT_BLACK);
  
  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(0, 0, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // We can now plot text on screen using the "print" class
  tft.println(getDateStr());
  tft.println(getTimeStr());
}

void runDisplayTime() {
  // TODO: Check for whether NTP has successfully updated
  timeToScreen();
}
