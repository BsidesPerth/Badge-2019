// NTP time sync and time display

// NTP client uses a UDP connection
WiFiUDP ntpUDP;

// Perth timezone offset (+8 hours) in seconds
const int timeOffset = 28800;

// Connect to Australian NTP time server with Perth timezone
NTPClient timeClient(ntpUDP, "au.pool.ntp.org", timeOffset);


bool timeServerSetup = false;
bool timeValid = false;

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
      bool result = timeClient.forceUpdate();

      if (result) {
        timeValid = true;
        Serial.print("{TIME} Updated time: ");
        Serial.print(getDateStr());
        Serial.print(" ");
        Serial.print(getTimeStr());
        Serial.println();
      } else {
        Serial.println("{TIME} Update failed");
      }      
    }
  }
}

String getTimeStr() {
  return timeClient.getFormattedTime();
}

// From: https://github.com/taranais/NTPClient/blob/master/NTPClient.cpp
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
  if (timeValid) {
    timeToScreen();
  }
}





// ========= TIME LIBRARY THINGS ==========

// Inspired by https://forum.arduino.cc/index.php?topic=465881.0
tmElements_t createElements(const char *str)
{
  tmElements_t tm;
  int y, m, d, H, M, S;
  sscanf(str, "%d-%d-%d %d:%d:%d", &y, &m, &d, &H, &M, &S);
  tm.Year = CalendarYrToTm(y);
  tm.Month = m;
  tm.Day = d;
  tm.Hour = H;
  tm.Minute = M;
  tm.Second = S;
  return tm;
}

// https://github.com/PaulStoffregen/Time/blob/master/Time.cpp
static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
time_t makeTime(const tmElements_t &tm){   
// assemble time elements into time_t 
// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
  
  int i;
  uint32_t seconds;

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds= tm.Year*(SECS_PER_DAY * 365);
  for (i = 0; i < tm.Year; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }
  
  // add days for this year, months start from 1
  for (i = 1; i < tm.Month; i++) {
    if ( (i == 2) && LEAP_YEAR(tm.Year)) { 
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
    }
  }
  seconds+= (tm.Day-1) * SECS_PER_DAY;
  seconds+= tm.Hour * SECS_PER_HOUR;
  seconds+= tm.Minute * SECS_PER_MIN;
  seconds+= tm.Second;
  return (time_t)seconds; 
}

void breakTime(time_t timeInput, tmElements_t &tm){
// break the given time_t into time components
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!

  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)timeInput;
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24; // now it is days
  tm.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1 
  
  year = 0;  
  days = 0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  tm.Year = year; // year is offset from 1970 
  
  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0
  
  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }
    
    if (time >= monthLength) {
      time -= monthLength;
    } else {
        break;
    }
  }
  tm.Month = month + 1;  // jan is month 1  
  tm.Day = time + 1;     // day of month
}
