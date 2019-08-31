// Show the sessions

//const char* sessionsUrl = "http://m.uploadedit.com/bbtc/1566736690361.txt";
//const char* sessionsUrl = "http://m.uploadedit.com/bbtc/1567180145660.txt";
const char* sessionsUrl = "http://m.uploadedit.com/bbtc/1567259330389.txt";
const char* sessionsFilename = "/sessions.txt";

const char* venueNames[2] = {
  "Westfarmers Lecture Theatre",
  "Ernst & Young Lecture Theatre"
};

extern NTPClient timeClient;

struct session {
  int venue;
  unsigned long datetime;
  String title;
  String speaker;
};

const int sessionListSize = 20;
session sessionList[sessionListSize];
int sessionCount = 0;

void readSessionListFromFlash() {
  Serial.println("{SESSIONS} parseSessionListFromString");
  String diskFile = readFile(sessionsFilename);
  if (diskFile.length() > 0) {
    parseSessionListFromString(diskFile);
  }  
}

void parseSessionListFromString(String & sessionListStr) {
  Serial.println("{SESSIONS} parseSessionListFromString");
  const char * itemSep = ",";
  const char * lineSep = "\n";
  int start = 0;
  int end = 0;
  String item;
  sessionCount = 0;
  for (int i=0; i<sessionListSize; i++) {
    if (!parseNextItem(start, end, sessionListStr, itemSep, item)) break;
    Serial.printf("{SESSIONS} venue = %s\n", item.c_str());
    sessionList[i].venue = item.toInt();

//    start = end;
//    end = sessionListStr.indexOf(delim, start);
//    if (end == -1) break;
//    String timeStr = sessionListStr.substring(start, end);
//    Serial.printf("{SESSIONS} timeStr (%d) = %s\n", end, timeStr.c_str());
//    sessionList[i].time = makeTime(createElements(timeStr.c_str()));
//    Serial.printf("{SESSIONS} time = %d\n", (int)sessionList[i].time);
//    end += 1;

    if (!parseNextItem(start, end, sessionListStr, itemSep, item)) break;
    Serial.printf("{SESSIONS} datetime = %s\n", item.c_str());
    sessionList[i].datetime = makeTime(createElements(item.c_str()));

//    start = end;
//    end = sessionListStr.indexOf(delim, start);
//    if (end == -1) break;
//    String name = sessionListStr.substring(start, end);
//    Serial.printf("{SESSIONS} name (%d) = %s\n", end, name.c_str());
//    sessionList[i].name = name;
//    end += 1;

    if (!parseNextItem(start, end, sessionListStr, itemSep, item)) break;
    Serial.printf("{SESSIONS} title = %s\n", item.c_str());
    sessionList[i].title = item;
    
    if (!parseNextItem(start, end, sessionListStr, lineSep, item)) break;
    Serial.printf("{SESSIONS} speaker = %s\n", item.c_str());
    sessionList[i].speaker = item;

//    start = end;
//    end = sessionListStr.indexOf(line, start);
//    if (end == -1) break;
//    String description = sessionListStr.substring(start, end);
//    Serial.printf("{SESSIONS} description (%d) = %s\n", end, description.c_str());
//    sessionList[i].description = description;
//    end += 1;

    sessionCount += 1;
  }  
}

// Look for "search" in "str" and output in "item".
// Return false if failed to find.
bool parseNextItem(int& start, int& end, const String& str, const char* search, String& item) {
  start = end;
  end = str.indexOf(search, start);
  if (end == -1) return false;  // No match, exit
  item = str.substring(start, end);
  end += 1;
  return true;
}


// Contact website and download list of sessions. Store list in SPIFFS.
void updateSessionsList()
{
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("{SESSIONS} Get sessions list from website");
    
    String sessionListStr;
    downloadSessionsList(sessionListStr);

    if (sessionListStr.length() > 0) {
      writeSessionListIfNew(sessionListStr);
    } else {
      Serial.println("{SESSIONS} No list received");
    }
  } else {
    Serial.println("{SESSIONS} No wifi connection, cannot update");
  }
}


void downloadSessionsList(String & sessionListStr) {
  // Use HTTPClient to do web requests
  HTTPClient http;
  Serial.print("[SESSIONS] Setup HTTP Request: ");
  Serial.println(sessionsUrl);
  //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
  http.begin(sessionsUrl); //HTTP

  Serial.flush();
  Serial.print("[SESSIONS] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.flush();
    Serial.printf("[SESSIONS] HTTP Response Code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      sessionListStr = http.getString();
    }
  } else {
    Serial.flush();
    Serial.printf("[SESSIONS] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void writeSessionListIfNew(String & sessionListStr) {
  // ESP32 debug logging was clobbering following log lines
  // flush() clears things up
  Serial.flush();
  
  // Dump response
  Serial.println("{SESSIONS) Response text:");
  Serial.println(sessionListStr);
  
  // Check for changes
  String diskFile = readFile(sessionsFilename);
  if (diskFile.length() > 0) {
    Serial.println("{SESSIONS) Currently on badge:");
    Serial.println(diskFile);
  }
  if (!diskFile.equals(sessionListStr)) {
    // File changed so save to disk
    Serial.println("{SESSIONS} Writing list to file");
    writeFile(sessionsFilename, sessionListStr.c_str());

    // List new so parse it into memory
    Serial.println("{SESSIONS} Parsing into memory");
    parseSessionListFromString(sessionListStr);
  } else {
    Serial.println("{SESSIONS} No change to sessions file");
  }
  
}

// ============== Functions to display sessions list (session times) =============


bool enableSessionsDisplay() {
  Serial.println("{SESSIONS} Enable Sessions Display");
  tft.fillScreen(TFT_NAVY);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawCentreString("Session Times", 120, 0, 4);

  Serial.println("string converted to tmElements.");
  Serial.println("format is yyyy-mm-dd hh:mm:ss");
  char firstDateTime[] = {"2017-03-26 10:30:15"};
  char secondDateTime[] = {"2017-03-27 10:30:15"};
  Serial.println(firstDateTime);
  Serial.println(secondDateTime);
  auto myTime = createElements(firstDateTime);
  unsigned long firstTime = makeTime(myTime);
  myTime = createElements(secondDateTime);
  unsigned long secondTime = makeTime(myTime);
  Serial.println("Time Difference in Minutes");
  Serial.println((secondTime - firstTime)/60);

  
  return true;
}

void disableSessionsDisplay() {
  
}

void loopSessionsDisplay() {
//  // Now print text on top of the graphics
//  img.setTextSize(1);           // Font size scaling is x1
//  img.setTextFont(4);           // Font 4 selected
//  img.setTextColor(TFT_BLACK);  // Black text, no background colour
//  img.setTextWrap(false);       // Turn of wrap so we can print past end of sprite
//
//  // Need to print twice so text appears to wrap around at left and right edges
//  img.setCursor(xpos, 2);  // Print text at xpos
//  img.print(msg);
//
//  img.setCursor(xpos - IWIDTH, 2); // Print text at xpos - sprite width
//  img.print(msg);
}
