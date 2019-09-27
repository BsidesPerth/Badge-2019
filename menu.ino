//
// Display a menu for accessing badge modes and features
//

// Currently selected menu item
int menuSelection = 0;
bool menuNeedsRefresh = true;

struct TMenuEntry {
  const char * name;
  Task * task;
};

const int menuCount = 7;
TMenuEntry menuEntries[menuCount] = {
  {"Name Tag", &tNameTag},
  {"Edit Name", &tNameEdit},
  {"Session Times", &tSessionsDisplay},
  {"Badge Em All", &tBadgeem},
  {"Game of Life", &tGameOfLife},
  {"Sponsors", &tImagesDisplay},
  {"Self-Check", &tCheckLoop}
};

bool menuEnable() {
  Serial.println(F("{MENU} Switch to Menu"));
  tft.fillScreen(TFT_NAVY);
  tft.setTextSize(1);
  img.setTextColor(TFT_WHITE);
  img.setColorDepth(8);
  img.setTextWrap(false);

  tft.setTextColor(TFT_YELLOW, TFT_YELLOW);
  tft.drawCentreString("Menu", 120, 3, 4);

  for (int i=0; i<NUMBER_BUTTONS; i++) {
    buttons[i].callback = idleButtonHandler;
  }  

  menuRedraw();

  return true;
}

void idleButtonHandler(EBUTTONS button, bool pressed) {
  if (pressed && (button == BUTTON_UP)) {
    // Previous item
    menuSelection -= 1;
    if (menuSelection < 0) menuSelection = menuCount - 1;
    menuRedraw();
  } else if (pressed && (button == BUTTON_DOWN)) {
    // Next item
    menuSelection += 1;
    menuSelection %= menuCount;
    menuRedraw();
  } else if (pressed && (button == BUTTON_BOTTOM)) {
    // Select item
    auto & selected = menuEntries[menuSelection];
    if (selected.task) {
      Serial.print(F("{MENU} Select "));
      Serial.println(selected.name);
      tMenu.disable();
      selected.task->enable();
    }
  }
}

void menuLoop() {}

void menuRedraw() {
  menuNeedsRefresh = false;
  Serial.print(F("{MENU} Refresh "));
  Serial.println(menuSelection);
  
  const int vertSpace = 30;
  const int boxWidth = 130;
  const int leftPos = (240-boxWidth)/2;
  for (int i=0; i<menuCount; i++) {
    menuDrawItem(menuEntries[i].name, i==menuSelection, leftPos, 35 + vertSpace*i, boxWidth);
  }
}

void menuDrawItem(const char * name, bool selected, int x, int y, int width) {
  img.createSprite(width, 20);
  const int backCol = selected ? TFT_DARKCYAN : TFT_NAVY;
  //const int foreCol = selected ? TFT_BLACK : TFT_WHITE;
  const int foreCol = TFT_WHITE;
  img.fillSprite(backCol);
  img.setTextColor(foreCol, foreCol);
  img.drawCentreString(name, width/2, 3, 2);
  img.pushSprite(x, y);
  img.deleteSprite();
}
