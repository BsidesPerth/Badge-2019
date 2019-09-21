//
// Badge Em All
//

enum BADGEEM_MODE {
  BM_HOME,
  BM_RECEIVE,
  BM_SEND
};
BADGEEM_MODE badgeemMode = BM_HOME;

const char* badgeemUrl = "https://a-z.pw:5000";
const char* BMUregister = "/register";
const char* BMUsubmit = "/submit";
const char* BMUupdate_name = "/update_name";
const char* BMUstatus = "/status";

const char* badgeemRegisterFile = "/badgeem_register.txt";

const int capacity = JSON_OBJECT_SIZE(10);  // 4 fields plus a spare
StaticJsonDocument<capacity> json;

bool badgeemRegistered = false;
String badgeemToken;
String badgeemHash;
String badgeemImageUrl;


const char* ca = "-----BEGIN CERTIFICATE-----\n"
                 "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw"
                 "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh"
                 "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4"
                 "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu"
                 "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY"
                 "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc"
                 "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+"
                 "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U"
                 "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW"
                 "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH"
                 "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC"
                 "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv"
                 "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn"
                 "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn"
                 "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw"
                 "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI"
                 "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV"
                 "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq"
                 "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL"
                 "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ"
                 "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK"
                 "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5"
                 "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur"
                 "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC"
                 "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc"
                 "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq"
                 "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA"
                 "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d"
                 "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc="
                 "-----END CERTIFICATE-----\n";



bool badgeemEnable() {
  Serial.println(F("Badge Em All"));
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_YELLOW);
  tft.drawCentreString(F("Badge Em All"), 120, 0, 4);

  for (int i = 0; i < NUMBER_BUTTONS; i++) {
    buttons[i].callback = badgeemButtonHandler;
  }

  tLedRainbow.disable();

  return true;
}

void badgeemDisable() {
  tLedRainbow.enable();
}

void badgeemButtonHandler(EBUTTONS button, bool pressed) {
  if (pressed && (button == BUTTON_MID)) {
    tBadgeem.disable();
    tMenu.enable();
  } else if (pressed && (button == BUTTON_BOTTOM)) {
    // Ok
    //badgeemRegister();
  }

  // Event driven check update
  badgeemLoop();
}

void badgeemLoop() {
  Serial.println(F("Badgeem Loop"));
  if (badgeemMode == BM_HOME) {
    // Display data from server
    if (badgeemRegistered) {
      int score = badgeemDownloadScore();
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawString(String("Score = ") + String(score), 10, 30, 4);
    } else {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.drawString("Not Registered", 10, 30, 4);
    }
  }
}

//=======================================================================
// Score

int badgeemDownloadScore() {
  int score = -1;
  
  if (WiFi.status() != WL_CONNECTED) {
    return score;
  }
  
  // Use HTTPClient to do web requests
  HTTPClient http;
  String requestStr = String(badgeemUrl) + BMUstatus + "?badge_token=" + badgeemToken;
  Serial.print("[badgeemDownloadScore] requestStr = ");
  Serial.println(requestStr);
  http.begin(requestStr, ca); //HTTPS

  // start connection and send HTTP header
  //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//  String content = "address=";
//  uint64_t chipid = ESP.getEfuseMac();
//  content += String((uint16_t)(chipid >> 32), HEX); // High 2 bytes
//  content += String((uint32_t)chipid, HEX);// Low 4 bytes
//  Serial.println("POSTing");
//  Serial.println(content);
  //int httpCode = http.POST(content);
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.flush();
    Serial.printf("[badgeemDownloadScore] HTTP Response Code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String text = http.getString();
      Serial.println(text);
      score = badgeemParseScore(text);
    }
  } else {
    Serial.flush();
    Serial.printf("[badgeemDownloadScore] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  return score;
}

int badgeemParseScore(const String & str) {
  Serial.print("[badgeemParseScore] parse score");
  Serial.println(str);
  DeserializationError err = deserializeJson(json, str);
  int score = -1;

  if (!err) {
    String status = json["status"];
    if (status == "OK") {
      Serial.println("Parse ok!");
      Serial.flush();
      score = json["score"];
      Serial.print("Score=");
      Serial.println(score);
    } else {
      Serial.print(F("[badgeemParseScore] Bad status = "));
      Serial.println(status);
    }
  } else {
    Serial.print(F("[badgeemParseScore] Deserialisation error = "));
    Serial.println(err.c_str());
  }

  return score;
}


//=======================================================================
// Registration

void badgeemRegisterLoop() {
  if (!badgeemRegistered) {
    // Try to load from file
    String registerStr = readFile(badgeemRegisterFile);
    if (registerStr.length() > 0) {
      // File has contents, try and read
      badgeemParseRegisterStr(registerStr);
    }

    // Try to register on server if didn't load anything valid
    if (!badgeemRegistered) {
      badgeemRegisterToServer();
    }
  }
}

void badgeemRegisterToServer() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  // Use HTTPClient to do web requests
  HTTPClient http;
  Serial.println(F("[BADGEEM] Register"));
  http.begin(String(badgeemUrl) + BMUregister, ca); //HTTPS

  // start connection and send HTTP header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String content = "address=";
  uint64_t chipid = ESP.getEfuseMac();
  content += String((uint16_t)(chipid >> 32), HEX); // High 2 bytes
  content += String((uint32_t)chipid, HEX);// Low 4 bytes
  Serial.println("POSTing");
  Serial.println(content);
  int httpCode = http.POST(content);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.flush();
    Serial.printf("[BADGEEM] HTTP Response Code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String text = http.getString();
      Serial.println(text);
      badgeemParseRegisterStr(text);
      if (badgeemRegistered) {
        // Write to flash as string
        writeFile(badgeemRegisterFile, text.c_str());
      }
    }
  } else {
    Serial.flush();
    Serial.printf("[BADGEEM] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void badgeemParseRegisterStr(const String & str) {
  DeserializationError err = deserializeJson(json, str);

  if (!err) {
    String status = json["status"];
    if (status == "OK") {
      Serial.println("Parse ok!");
      Serial.flush();
      badgeemRegistered = true;
      badgeemHash = json["hash"].as<String>();
      badgeemImageUrl = json["image_url"].as<String>();
      badgeemToken = json["token"].as<String>();
      Serial.print("Hash=");
      Serial.println(badgeemHash);
      Serial.print("ImageURL=");
      Serial.println(badgeemImageUrl);
      Serial.print("Token=");
      Serial.println(badgeemToken);
    } else {
      Serial.print(F("[badgeemParseRegisterStr] Bad status = "));
      Serial.println(status);
    }
  } else {
    Serial.print(F("[badgeemParseRegisterStr] Deserialisation error = "));
    Serial.println(err.c_str());
  }
}
