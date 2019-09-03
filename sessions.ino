// Show the sessions

//const char* sessionsUrl = "http://m.uploadedit.com/bbtc/1566736690361.txt";
//const char* sessionsUrl = "http://m.uploadedit.com/bbtc/1567180145660.txt";
const char* sessionsUrl = "http://m.uploadedit.com/bbtc/1567259330389.txt";
//const char* sessionsUrl = "http://m.uploadedit.com/bbtc/bad.txt";
const char* sessionsFilename = "/sessions.txt";

const char* venueNames[4] = {
  "Westfarmers Lecture Theatre",
  "Ernst & Young Lecture Theatre",
  "Lobby",
  "Workshop"
};

extern NTPClient timeClient;

struct Session_t {
  int venue;
  time_t datetime;
  String title;
  String speaker;
};
struct SessionPairs_t {
  Session_t *first;
  Session_t *second;
};

const int sessionListSize = 40;
Session_t sessionList[sessionListSize];
int sessionCount = 0;
SessionPairs_t sessionPairList[sessionListSize];
int sessionPairCount = 0;   

//prototypes
void drawSessionOverview(Session_t * session, int y);

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

  bool firstOfPair = true;
  
  for (int i=0; i<sessionListSize; i++) {
    if (!parseNextItem(start, end, sessionListStr, itemSep, item)) break;
    Serial.printf("{SESSIONS} venue = %s\n", item.c_str());
    sessionList[i].venue = item.toInt();

    if (!parseNextItem(start, end, sessionListStr, itemSep, item)) break;
    Serial.printf("{SESSIONS} datetime = %s\n", item.c_str());
    sessionList[i].datetime = strToTime(item);

    // Pair up those events with matching datetime
    if (firstOfPair) {
      // This i is a first, next one is second
      firstOfPair = false;
      // Store first in fresh slot in pairs list
      // Put it in pairs list. If no match then this becomes a single.
      SessionPairs_t pair;
      pair.first = &sessionList[i];
      pair.second = nullptr;
      sessionPairList[sessionPairCount] = pair;
      sessionPairCount += 1;      
    } else {
      // This i is a second so lets see if time matches the first
      if (sessionList[i].datetime == sessionPairList[sessionPairCount-1].first->datetime) {
        // We have a matching pair, save the second
        sessionPairList[sessionPairCount-1].second = &sessionList[i];        
        // The next one will be a first
        firstOfPair = true;
      } else {
        // They don't match. Make this a new first
        SessionPairs_t pair;
        pair.first = &sessionList[i];
        pair.second = nullptr;
        sessionPairList[sessionPairCount] = pair;
        sessionPairCount += 1;      
        // Next one is a candidate second
        firstOfPair = false;
      }
    }

    // Increment count here since we have put session in pairs list so it will be used.
    // A parsing error below will mean blank entries.
    sessionCount += 1;

    if (!parseNextItem(start, end, sessionListStr, itemSep, item)) break;
    Serial.printf("{SESSIONS} title = %s\n", item.c_str());
    sessionList[i].title = item;
    
    if (!parseNextItem(start, end, sessionListStr, lineSep, item)) break;
    Serial.printf("{SESSIONS} speaker = %s\n", item.c_str());
    sessionList[i].speaker = item;

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

    // Refresh display if currently displaying session times
    if (tSessionsDisplay.isEnabled()) {
      Serial.println("{SESSIONS} Refresh screen");
      enableSessionsDisplay();
    }
  } else {
    Serial.println("{SESSIONS} No change to sessions file");
  }
  
}

/////////////////////////////////////////////////////////////////////////////////////
// ============== Functions to display sessions list (session times) ============= //

// Currently displayed session
int sessPairDisp = 2;
bool sessionDisplayNeedsRefresh = true;

bool enableSessionsDisplay() {
  Serial.println("{SESSIONS} Enable Sessions Display");
  tft.fillScreen(TFT_NAVY);
  tft.setTextSize(1);
  img.setTextColor(TFT_WHITE);
  img.setColorDepth(1);
  img.setTextWrap(false);
  sessionDisplayNeedsRefresh = true;

  for (int i=0; i<NUMBER_BUTTONS; i++) {
    buttons[i].callback = sessionButtonHandler;
  }  

  return true;
}

void sessionButtonHandler(EBUTTONS button, bool pressed) {
  if (pressed && (button == BUTTON_RIGHT)) {
    // Next session
    sessPairDisp += 1;
    if (sessPairDisp >= sessionPairCount) {
      sessPairDisp = sessionPairCount - 1;
    }
  } else if (pressed && (button == BUTTON_LEFT)) {
    // Prev session
    sessPairDisp -= 1;
    if (sessPairDisp < 0) {
      sessPairDisp = 0;
    }
  }
  
  // Event driven display update
  sessionDisplayNeedsRefresh = true;
  loopSessionsDisplay();
}

void disableSessionsDisplay() {
  
}

void loopSessionsDisplay() {
  if (sessionDisplayNeedsRefresh) {
    tft.fillScreen(TFT_NAVY);
    sessionDisplayNeedsRefresh = false;
    
    if (sessionCount == 0) {
      // Report no sessions
      sessionsDisplayNone();
    } else {
      Serial.printf("{SESSIONS} Loop Sessions Display: Refresh %d", sessPairDisp);
      if (sessPairDisp == -1) {
        // Display welcome page
        sessionsDisplayWelcome();
        // - refresh again for time update
        sessionDisplayNeedsRefresh = true;
      } else {
        // Display the selected session
        sessionDisplayParent();
      }
    }
  }
}

// Screen for no sessions loaded
void sessionsDisplayNone() {
  tft.setTextColor(TFT_YELLOW, TFT_YELLOW);
  tft.drawCentreString("Session Times", 115, 3, 4);

  img.createSprite(240, 60);
  img.fillSprite(TFT_BLACK);
  img.setTextColor(TFT_WHITE);
  img.drawCentreString("No Sessions", 120, 0, 4);
  img.drawCentreString("Loaded", 120, 30, 4);
  // Specify the colours of the ON and OFF pixels
  tft.setBitmapColor(TFT_RED, TFT_TRANSPARENT);
  img.pushSprite(0, 80, TFT_TRANSPARENT);
  img.deleteSprite();
}

// Welcome screen for session interface, explaining how it works and showing current time
void sessionsDisplayWelcome() {
  tft.setTextColor(TFT_YELLOW, TFT_YELLOW);
  tft.drawCentreString("Session Times", 115, 3, 4);

  // - help text
  const int offset = 5;
  img.createSprite(230, 130);
  img.fillSprite(TFT_BLACK);
  img.setTextFont(2);
  img.println("Use        to browse sessions");
  img.println();
  img.println("Use        to select a room");
  img.println();
  img.println("Use A (SW8) to mark a talk and");
  img.println("badge will remind you 15 min");
  img.print(  "before start");
  // Specify the colours of the ON and OFF pixels
  tft.setBitmapColor(TFT_WHITE, TFT_TRANSPARENT);
  img.pushSprite(5, 40+offset, TFT_TRANSPARENT);
  img.deleteSprite();

  // - arrows
  drawArrow(38, 48+offset, 15, 270, TFT_WHITE);
  drawArrow(60, 48+offset, 15, 90, TFT_WHITE);

  drawArrow(38, 80+offset, 15, 0, TFT_WHITE);
  drawArrow(60, 80+offset, 15, 180, TFT_WHITE);

  drawArrow(220, 15, 20, 90, TFT_YELLOW);
  
  // - datetime label
  img.createSprite(230, 30);
  img.fillSprite(TFT_BLACK);
  img.drawString("Current Time:", 0, 0, 4);
  // Specify the colours of the ON and OFF pixels
  tft.setBitmapColor(TFT_GREENYELLOW, TFT_TRANSPARENT);
  img.pushSprite(3, 185, TFT_TRANSPARENT);
  img.deleteSprite();

  // - datetime
  img.createSprite(230, 25);
  img.fillSprite(TFT_BLACK);
  time_t t = timeClient.getEpochTime();
  img.drawString(timeToStr(t), 0, 0, 4);
  // Specify the colours of the ON and OFF pixels
  tft.setBitmapColor(TFT_GREENYELLOW, TFT_NAVY);
  img.pushSprite(3, 215);
  img.deleteSprite();
}

void sessionDisplayParent() {
  // - datetime
  img.createSprite(240, 25);
  img.fillSprite(TFT_BLACK);
  img.setTextColor(TFT_WHITE);
  img.drawCentreString(timeToStr(sessionPairList[sessPairDisp].first->datetime), 120, 0, 4);
  tft.setBitmapColor(TFT_YELLOW, TFT_TRANSPARENT);
  img.pushSprite(0, 3, TFT_TRANSPARENT);
  img.deleteSprite();

  drawArrow(10,   13, 20, 270, TFT_YELLOW);
  drawArrow(230, 13, 20, 90,  TFT_YELLOW);
  
  drawSessionOverview(sessionPairList[sessPairDisp].first, 25);

  if (sessionPairList[sessPairDisp].second) {
    drawSessionOverview(sessionPairList[sessPairDisp].second, 108+25);
  }
}

// Draw individual session
void drawSessionOverview(Session_t * session, int y) {
  // 105 high is budget

  // Divider line that venue is drawn on
  //tft.drawLine(0, y+0, 240, y+0, TFT_YELLOW);
  tft.drawFastHLine(0, y+0, 240, TFT_YELLOW);

  // Venue
  String venueName = venueNames[session->venue - 1];
  img.createSprite(tft.textWidth(venueName, 2) + 6, tft.fontHeight(2));
  img.fillSprite(TFT_BLACK);
  img.drawCentreString(venueName, img.width()/2, 0, 2);
  tft.setBitmapColor(TFT_CYAN, TFT_NAVY);
  img.pushSprite((240-img.width())/2, y+2);
  img.deleteSprite();
  
  // Title (2 line)
  int h = tft.fontHeight(2)*2 + tft.fontHeight(4) + 5;
  img.createSprite(240, h);
  img.fillSprite(TFT_BLACK);
  //String multilineText = wrapWordsAtSpaces(sessionList[sessionIdx].title, 2, 240);
  img.setCursor(0, 0);
  //img.setTextFont(2);
  //img.print(multilineText);
  printMultilineWrapAtSpaces(session->title, 4, 2, 240);
  tft.setBitmapColor(TFT_WHITE, TFT_TRANSPARENT);
  img.pushSprite(0, y+25, TFT_TRANSPARENT);
  img.deleteSprite();
  
  // Speaker name
  img.createSprite(240, tft.fontHeight(2));
  img.fillSprite(TFT_BLACK);
  img.drawString(session->speaker, 0, 0, 2);
  tft.setBitmapColor(TFT_GREENYELLOW, TFT_TRANSPARENT);
  img.pushSprite(0, y+25+h+3, TFT_TRANSPARENT);
  img.deleteSprite();
}

//out.length() == 0 ? font1, font2
void printMultilineWrapAtSpaces(const String &str, int font1, int font2, int width) {
  int pos = 0;
  String line;
  String nextWord;
  int font = font1;
  img.setTextFont(font);
  while (true) {
    int next = str.indexOf(" ", pos);
    if (next != -1) {
      nextWord = str.substring(pos, next);
    } else {
      // Put the rest on line
      nextWord = str.substring(pos);
    }

    if (tft.textWidth(line + nextWord, font) > width) {
      img.println(line);
      line = "";
      font = font2;
      img.setTextFont(font);
    }
    line += nextWord + " ";

    if (next != -1) {
      pos = next+1;
    } else {
      break;
    }
  }
  img.print(line);
}

String wrapWordsAtSpaces(const String &str, int font, int width) {
  int pos = 0;
  String out;
  String line;
  String nextWord;
  while (true) {
    int next = str.indexOf(" ", pos);
    if (next != -1) {
      nextWord = str.substring(pos, next);
    } else {
      // Put the rest on line
      nextWord = str.substring(pos);
    }

    if (tft.textWidth(line + nextWord, font) > width) {
      out += line + "\n";
      line = "";
    }
    line += nextWord + " ";

    if (next != -1) {
      pos = next+1;
    } else {
      break;
    }
  }
  out += line;
  return out;
}


// SCROLLING TEXT
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


// Arrow pointed up rotated by rot in degrees
void drawArrow(int x0, int y0, int len, int rot, int col) {
  img.setColorDepth(8);
  const int width = len/2;
  img.createSprite(width, len);
  img.fillSprite(TFT_TRANSPARENT);
  
  img.drawLine(width/2, 0, width/2, len, col);
  img.drawLine(width/2, 0, 0, len/2, col);
  img.drawLine(width/2, 0, width-1, len/2, col);

  tft.setPivot(x0, y0);
  img.setPivot(img.width()/2, img.height()/2);
  img.pushRotated(rot, TFT_TRANSPARENT);
  img.deleteSprite();
  img.setColorDepth(1);
}
