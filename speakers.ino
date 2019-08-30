// Show the speakers and sessions

//const char* speakersUrl = "http://m.uploadedit.com/bbtc/1566736690361.txt";
const char* speakersUrl = "http://m.uploadedit.com/bbtc/1567180145660.txt";
const char* speakersFilename = "/speakers.txt";

extern NTPClient timeClient;

struct speaker_t {
  unsigned long time;
  String name;
  String description;
};

const int speakerListSize = 20;
speaker_t speakerList[speakerListSize];
int speakerCount = 0;
bool speakerListRead = false;

// Contact website and download list of speakers. Store list in SPIFFS.
void updateSpeakersList()
{
  if (!speakerListRead) {
    // Read if there is anything there and don't try again
    readSpeakerListFromFile();
    speakerListRead = true;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("{SPEAKERS} Get speakers list from website");
    
    String speakerListStr;
    downloadSpeakersList(speakerListStr);

    if (speakerListStr.length() > 0) {
      writeSpeakerListIfNew(speakerListStr);
    } else {
      Serial.println("{SPEAKERS} No list received");
    }
  } else {
    Serial.println("{SPEAKERS} No wifi connection, cannot update");
  }
}

void readSpeakerListFromFile() {
  String diskFile = readFile(speakersFilename);
  if (diskFile.length() > 0) {
    parseSpeakerListFromString(diskFile);
  }  
}

void parseSpeakerListFromString(String & speakerListStr) {
  Serial.println("parseSpeakerListFromString");
  const char * delim = ",";
  const char * line = "\n";
  int start = 0;
  int end = 0;
  speakerCount = 0;
  for (int i=0; i<speakerListSize; i++) {
    start = end;
    end = speakerListStr.indexOf(delim, start);
    if (end == -1) break;
    String timeStr = speakerListStr.substring(start, end);
    Serial.printf("timeStr (%d) = %s\n", end, timeStr.c_str());
    speakerList[i].time = makeTime(createElements(timeStr.c_str()));
    Serial.printf("time = %d\n", speakerList[i].time);
    end += 1;

    start = end;
    end = speakerListStr.indexOf(delim, start);
    if (end == -1) break;
    String name = speakerListStr.substring(start, end);
    Serial.printf("name (%d) = %s\n", end, name.c_str());
    speakerList[i].name = name;
    end += 1;

    start = end;
    end = speakerListStr.indexOf(line, start);
    if (end == -1) break;
    String description = speakerListStr.substring(start, end);
    Serial.printf("name (%d) = %s\n", end, description.c_str());
    speakerList[i].description = description;
    end += 1;
//    start = end;
//    end = speakerListStr.indexOf(delim);
//    speakersList[i].description = speakerListStr.substring(start, end);

    speakerCount += 1;
  }  
}

void downloadSpeakersList(String & speakerListStr) {
  // Use HTTPClient to do web requests
  HTTPClient http;
  Serial.print("[SPEAKERS] Setup HTTP Request: ");
  Serial.println(speakersUrl);
  //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
  http.begin(speakersUrl); //HTTP

  Serial.flush();
  Serial.print("[SPEAKERS] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.flush();
    Serial.printf("[SPEAKERS] HTTP Response Code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      speakerListStr = http.getString();
    }
  } else {
    Serial.flush();
    Serial.printf("[SPEAKERS] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void writeSpeakerListIfNew(String & speakerListStr) {
  // ESP32 debug logging was clobbering following log lines
  // flush() clears things up
  Serial.flush();
  
  // Dump response
  Serial.println("{SPEAKERS) Response text:");
  Serial.println(speakerListStr);
  
  // Check for changes
  String diskFile = readFile(speakersFilename);
  if (diskFile.length() > 0) {
    Serial.println("{SPEAKERS) Currently on badge:");
    Serial.println(diskFile);
  }
  if (!diskFile.equals(speakerListStr)) {
    // File changed so save to disk
    Serial.println("{SPEAKERS} Writing list to file");
    //Serial.println("{SPEAKERS} NOT");
    writeFile(speakersFilename, speakerListStr.c_str());

    // List new so parse it into memory
    Serial.println("{SPEAKERS} Parsing into memory");
    parseSpeakerListFromString(speakerListStr);
  } else {
    Serial.println("{SPEAKERS} No change to speakers file");
  }
  
}

// ============== Functions to display speakers list (programme) =============


bool enableSpeakersDisplay() {
  Serial.println("Enable Speakers Display");
  tft.fillScreen(TFT_NAVY);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawCentreString("Programme", 120, 0, 4);

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

void disableSpeakersDisplay() {
  
}

void loopSpeakersDisplay() {
  //String diskFile = readFile(speakersFilename);
  //if (diskFile.length() > 0) {
  //Serial.println("Loop speaker display");
  //auto t = timeClient.getEpochTime();
  //Serial.printf("epochTime=%d, year=%d\n", t, year(t));
}
