// Name tag and name editor to show attendee name
// Acts as the idle screen shown when badge not in active use and puts the ESP32 to sleep

// TODO: Go to sleep and wait for button press/ wake up timer
// TODO: Set FastLED colours to screen colours - some difference between awake and sleep

const char * tagNameFile = "/name_tag.txt";

//======================================================

const int letterCount = 28;
const char LETTERS[letterCount] = {
  '<', ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};
int letterPos = 0;
String tagName = "";
const int TAG_NAME_MAX_LEN = 50;

//======================================================

const int tagColourCount = 10;
const int TAG_BK_COLS[tagColourCount] = {TFT_BLACK, TFT_LIGHTGREY, TFT_NAVY, TFT_NAVY,   TFT_DARKGREEN, TFT_PURPLE, TFT_PURPLE,      TFT_NAVY,      TFT_MAROON, TFT_DARKGREEN};
const int TAG_FG_COLS[tagColourCount] = {TFT_WHITE, TFT_BLACK,     TFT_BLUE, TFT_ORANGE, TFT_GREEN,     TFT_PINK,   TFT_GREENYELLOW, TFT_LIGHTGREY, TFT_GREEN,  TFT_RED};
int tagColPos = 0;


//======================================================

const int TAG_FONTS_COUNT = 4;
const GFXfont * TAG_FONTS[TAG_FONTS_COUNT] = {FSS24, FSS18, FSS12, FSS9};

bool nameTagEnable() {
  // Button handler
  for (int i=0; i<NUMBER_BUTTONS; i++) {
    buttons[i].callback = nameTagButtonHandler;
  }  

  // Colours as per user selection
  tft.fillScreen(TAG_BK_COLS[tagColPos]);
  tft.setTextColor(TAG_FG_COLS[tagColPos], TAG_BK_COLS[tagColPos]);
  tft.setTextWrap(false, false);

  if (tagName.length() > 0) {
    // Bsides title
    img.setFreeFont(FSS18);
    img.setColorDepth(1);
    if (img.createSprite(tft.width(), 50)) {
      Serial.println(F("{TAG} Sprite created"));
    } else {
      Serial.println(F("{TAG} Sprite creation failed"));
    }
    img.fillSprite(TFT_BLACK);
    img.setTextColor(TFT_WHITE);
    img.drawCentreString("BSides 2019", tft.width()/2, 12, GFXFF);
    img.setBitmapColor(TAG_BK_COLS[tagColPos], TAG_FG_COLS[tagColPos]);
    img.pushSprite(0, 0);
    img.deleteSprite();

    //tft.drawFastHLine(
    
    //tft.drawString(tagName, tft.width()/2, 50, GFXFF);// Print the string name of the font
    // User name tag
    int nameStartY = 70;
    // Check if it will fit
    if (printOneWordPerLineVariableHeight(tagName, tft.width(), nameStartY, true)) {
      printOneWordPerLineVariableHeight(tagName, tft.width(), nameStartY, false);
    } else {
      img.createSprite(tft.width()-20, tft.height() - nameStartY);
      img.fillSprite(TFT_BLACK);
      img.setCursor(0, 0);
      printMultilineWrapAtSpaces(tagName, 4, 4, tft.width()-20);
      img.setBitmapColor(TAG_FG_COLS[tagColPos], TAG_BK_COLS[tagColPos]);
      img.pushSprite(10, nameStartY);
      img.deleteSprite();
    }
  } else {
    // No name set default
    tft.setFreeFont(FSS24);
    //tft.setTextSize(2);
    tft.drawCentreString("BSides", tft.width()/2, 70, GFXFF);
    //tft.setTextSize(1);
    //tft.setTextFont(8);
    tft.drawCentreString("2019", tft.width()/2, 120, GFXFF);
  }
  
  return true;
}

bool printOneWordPerLineVariableHeight(const String &str, int width, int y, bool simulate) {
  int pos = 0;
  String nextWord;
  int h = y;
  
  while (true) {
    // Get next word in string (space separated)
    int next = str.indexOf(" ", pos);
    if (next != -1) {
      nextWord = str.substring(pos, next);
    } else {
      // The rest of string is next word
      nextWord = str.substring(pos);
    }

    // Find the largest font that fits
    int fontIndex = 0;
    for (int i=0; i<TAG_FONTS_COUNT; i++) {
      fontIndex = i;
      tft.setFreeFont(TAG_FONTS[fontIndex]);
      if (tft.textWidth(nextWord) < width) {
        // This word fits on screen at this font size.
        // Font size has been set.
        break;
      } // else: it doesn't fit on screen, try smaller font
    }

    if (!simulate) {
      if (tft.textWidth(nextWord) < width) {
        // Draw centred
        tft.drawCentreString(nextWord, width/2, h, GFXFF);
      } else {
        // Draw left aligned so right is cut off
        tft.drawString(nextWord, 0, h, GFXFF);
      }
    }

    // Move down by font height
    h += tft.fontHeight();

    if (h > tft.height()) {
      // Dont bother continuing if we are beyond screen.
      // Failed to fit on screen.
      return false;
    }

    // Advance pos to get it past found space and if no space then done
    if (next != -1) {
      pos = next+1;
    } else {
      break;
    }
  }
  return true;
}


void nameTagButtonHandler(EBUTTONS button, bool pressed) {
  if (pressed) {
    // On any button, return to menu
    tNameTag.disable();
    tMenu.enable();
  }
}

void nameTagLoop() {
//  // Background rainbow
//  const byte rainbowSaturation = 255; // 200
//  const byte rainbowIntensity = 250;
//  static int loopCounter = 0;
//  for (int i = 0; i < img.height(); i++) {
//    CRGB col = CHSV(i + loopCounter, rainbowSaturation, rainbowIntensity);
//    unsigned int colour = col.red/8 << 11 | col.green/4 << 5 | col.blue/8;
//    img.drawFastHLine(0, i, img.width(), colour);
//  }
//  loopCounter += 1;
//
//  // White text
//  // Do not plot the background colour by setting to same colour
//  img.setTextColor(TFT_WHITE, TFT_WHITE);
//  //img.setTextColor(TFT_YELLOW, TFT_YELLOW);
//  
//
//  img.setTextSize(3);
//  img.drawCentreString("Bsides", 115, 50, 4);
//  img.setTextSize(1);
//  img.drawCentreString("2019", 115, 130, 8);
//
//  // Draw to screen
//  img.pushSprite(5, 5);
}

///////////////////////////////////////////////////////////////////////////////////

// Prototypes
void changeLetter(bool increasing);
void writeTagToFlash();

int editRow = 0;
const int editRowCount = 2;

bool nameEditEnable() {
  Serial.println(F("{NAME_EDIT} Switch to Name Edit"));
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  img.setTextColor(TFT_WHITE);
  img.setColorDepth(8);
  img.setTextWrap(false);
  tft.setTextWrap(false);
  const int spacing = 5;

  editRow = 0;

  // Button handler
  for (int i=0; i<NUMBER_BUTTONS; i++) {
    buttons[i].callback = nameEditButtonHandler;
  }  

  // Draw static text
  
  tft.setTextColor(TFT_YELLOW, TFT_YELLOW);
  const int topGap = 3;
  tft.drawCentreString("Name Edit", 120, topGap, 4);
  int h = topGap + tft.fontHeight(4) + spacing;

  tft.setTextColor(TFT_CYAN, TFT_CYAN);
  tft.drawString("Enter Name:", 0, h, 4);
  h += tft.fontHeight(4) + spacing;

  // name
  h += tft.fontHeight(4) + spacing;

  // draw letters
  h += tft.fontHeight(4) + spacing;

  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawString("Note: Name is displayed on badge and ", 0, h, 2);
  h += tft.fontHeight(2);
  tft.drawString("leaderboard website", 0, h, 2);
  h += tft.fontHeight(2) + spacing + 20;

  tft.setTextColor(TFT_CYAN, TFT_CYAN);
  tft.drawString("Name Tag Colours:", 0, h, 4);
  h += tft.fontHeight(4) + spacing;

  // colours
  h += tft.fontHeight(4) + spacing;

  nameEditLoop();

  return true;
}

void nameEditDisable() {
  // Persist name/colour to disk on exit
  writeTagToFlash();
}

void nameEditButtonHandler(EBUTTONS button, bool pressed) {
  // TODO: SW6 switches between edit and display
  if (pressed && (button == BUTTON_DOWN) && (editRow < editRowCount-1)) {
    editRow += 1;
    nameEditLoop();
  }
  if (pressed && (button == BUTTON_UP) && (editRow > 0)) {
    editRow -= 1;
    nameEditLoop();
  }
  if (pressed && (button == BUTTON_LEFT)) {
    if (editRow == 0) {
      changeLetter(false);
    } else if (editRow == 1) {
      changeColour(false);
    }
    nameEditLoop();
  }
  if (pressed && (button == BUTTON_RIGHT)) {
    if (editRow == 0) {
      changeLetter(true);
    } else if (editRow == 1) {
      changeColour(true);
    }
    nameEditLoop();
  }
  if (pressed && (button == BUTTON_BOTTOM)) {
    if (editRow == 0) {
      selectLetter();
    }
    nameEditLoop();
  }
  if (pressed && (button == BUTTON_MID)) {
    // On back button, return to menu
    tNameEdit.disable();
    tMenu.enable();
  }
}

void drawTagColours(int);

void nameEditLoop() {
  Serial.println(F("{NAME_EDIT} Loop"));
  const int spacing = 5;

  const int topGap = 3;
  int h = topGap + tft.fontHeight(4) + spacing;

  // enter name
  h += tft.fontHeight(4) + spacing;
  
//  tft.setTextColor(TFT_WHITE, TFT_WHITE);
//  tft.drawString("<name....>", 0, h, 4);
  drawName(h);
  h += tft.fontHeight(4) + spacing;

  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  drawLetters(h);
  h += tft.fontHeight(4) + spacing;

  // name display note
  h += tft.fontHeight(2);
  h += tft.fontHeight(2) + spacing + 20;

  // name tag colours
  h += tft.fontHeight(4) + 1;

//  tft.setTextColor(TFT_WHITE, TFT_WHITE);
//  tft.drawCentreString("... Red Blue Pink ...", 120, h, 4);
  drawTagColours(h);
  h += tft.fontHeight(4) + spacing;
}

////////////////////////////////////////////////////////////////////////////

void changeLetter(bool increasing) {
  if (increasing) {
    letterPos += 1;
    letterPos %= letterCount;
  } else {
    letterPos -= 1;
    if (letterPos < 0) {
      letterPos += letterCount;
    }
  }
} 

void selectLetter() {
  if ((letterPos == 0)) {
    if  (tagName.length() > 0) {
      // Delete letter from string
      tagName = tagName.substring(0, tagName.length() - 1);
    }
  } else {
    if (tagName.length() < TAG_NAME_MAX_LEN) {
      tagName = tagName + String(LETTERS[letterPos]);
    }
  }
}

void drawLetters(int y) {
  const int letterOffset = 26;
  const int lettersShown = 9;
  const int lettersEachSide = lettersShown/2;
  for (int i = -lettersEachSide; i <= lettersEachSide; i++) {
    int pos = (i+letterPos) % letterCount;
    if (pos < 0) {
      pos += letterCount;
    }

    int x = 120 + i * letterOffset;
    
    drawLetter(x, y, pos, (editRow == 0) ? (pos == letterPos) : false);
  }
}

void drawLetter(int x, int y, int letterIndex, bool marked) {
  img.setColorDepth(8);
  const int letterWidth = 23;
  const int font = 4;
  img.createSprite(letterWidth, tft.fontHeight(font));
  img.fillSprite(marked ? TFT_BLUE : TFT_BLACK);
  char tmp[2] = {0, 0};
  tmp[0] = LETTERS[letterIndex];
  img.drawCentreString(tmp, letterWidth/2, 3, font);
  img.pushSprite(x - letterWidth/2, y);
  img.deleteSprite();
}

void drawName(int y) {
  img.setColorDepth(8);
  img.setTextColor(TFT_WHITE, TFT_WHITE);
  const int font = 4;
  img.createSprite(240, tft.fontHeight(font));
  img.fillSprite(editRow == 0 ? TFT_NAVY : TFT_BLACK);
  if (tft.textWidth(tagName, font) <= 240) {
    img.drawString(tagName, 0, 3, font);
  } else {
    img.drawRightString(tagName, 240, 3, font);
  }
  
  img.pushSprite(0, y);
  img.deleteSprite();
}

///////////////////////////////////////////////////////////////////

void drawTagColours(int y) {
  const int offset = 50;
  const int shown = 5;
  const int eachSide = shown/2;
  for (int i = -eachSide; i <= eachSide; i++) {
    int pos = (i+tagColPos) % tagColourCount;
    if (pos < 0) {
      pos += tagColourCount;
    }

    int x = 120 + i * offset;
    drawTagColour(x, y, pos, (editRow == 1) ? (pos == tagColPos) : false);
  }
}

void drawTagColour(int x, int y, int index, bool marked) {
  img.setColorDepth(16);
  const int width = 45;
  const int font = 4;
  const int height = tft.fontHeight(font) + 10;
  img.createSprite(width, height);
  img.fillSprite(TAG_BK_COLS[index]);
  img.setTextColor(TAG_FG_COLS[index], TAG_FG_COLS[index]);
  img.drawCentreString("T", width/2, 5, font);
  if (marked) {
    img.drawRect(5, 2, width-1-10, height-1-9, TAG_FG_COLS[index]);
    img.drawRect(6, 3, width-1-12, height-1-11, TAG_FG_COLS[index]);
  }
  img.pushSprite(x - width/2, y);
  img.deleteSprite();
}

void changeColour(bool increasing) {
  if (increasing) {
    tagColPos += 1;
    tagColPos %= tagColourCount;
  } else {
    tagColPos -= 1;
    if (tagColPos < 0) {
      tagColPos += tagColourCount;
    }
  }
} 

///////////////////////////////////////////////////////////////////

void readTagFromFlash() {
  Serial.print(F("{TAG} Read from Flash: "));
  Serial.println(tagNameFile);
  String diskFile = readFile(tagNameFile);
  if (diskFile.length() > 0) {
    parseTagFromString(diskFile);
  }  
}

// Prototype
bool parseNextItem(int& start, int& end, const String& str, const char* search, String& item);

void parseTagFromString(String & tagStr) {
  Serial.println(F("{TAG} parseTagFromString"));
  const char * lineSep = "\n";
  int start = 0;
  int end = 0;
  String item;
  sessionCount = 0;

  if (!parseNextItem(start, end, tagStr, lineSep, item)) return;
  Serial.print(F("{TAG}  Name = "));
  Serial.println(item);
  String tempName = item;

  if (!parseNextItem(start, end, tagStr, lineSep, item)) return;
  Serial.print(F("{TAG}  Colour Index = "));
  Serial.println(item);

  // Succeeded so store
  tagColPos = item.toInt();
  tagName = tempName;
}

void writeTagToFlash() {
  String fileContents = tagName + "\n" + String(tagColPos) + "\n";
  Serial.println(F("{TAG} Writing name tag: "));
  Serial.print(fileContents);
  writeFile(tagNameFile, fileContents.c_str());
}
