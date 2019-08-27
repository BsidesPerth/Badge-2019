// Show the speakers and sessions

const char* speakersUrl = "http://m.uploadedit.com/bbtc/1566736690361.txt";
//const char* speakersUrl = "http://m.uploadedit.com/bbtc/1566736690362.txt";
const char* speakersFilename = "/speakers.txt";

// Contact website and download list of speakers. Store list in SPIFFS.
void updateSpeakersList()
{
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
  } else {
    Serial.println("{SPEAKERS} No change to speakers file");
  }
  
}
