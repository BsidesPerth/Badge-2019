//Example included from TFT_eSPI
//modified for use in this badge program

//The Game of Life, also known simply as Life, is a cellular automaton
//devised by the British mathematician John Horton Conway in 1970.
// https://en.wikipedia.org/wiki/Conway's_Game_of_Life


#define GRIDX 240
#define GRIDY 240
//#define CELLXY 2
#define CELLXY 1

#define GEN_DELAY 0

//Current grid
//uint8_t grid[GRIDX][GRIDY];
uint8_t *grid;

//The new grid for the next generation
//uint8_t newgrid[GRIDX][GRIDY];
uint8_t *newgrid;

//Number of generations
#define NUMGEN 600

uint16_t genCount = 0;

bool golNew = false;

bool gameoflifeEnable() {
  Serial.println(F("start game of life"));
  Serial.flush();
  
  // Button handler
  for (int i=0; i<NUMBER_BUTTONS; i++) {
    buttons[i].callback = gameoflifeButtonHandler;
  }  

  golNew = true;
  grid = new uint8_t[GRIDX*GRIDY];
  newgrid = new uint8_t[GRIDX*GRIDY];
  return true;
}

void gameoflifeButtonHandler(EBUTTONS button, bool pressed) {
  if (pressed) {
    // On any button, return to menu
    tGameOfLife.disable();
    tMenu.enable();
  }
}

void gameoflifeDisable() {
  delete grid;
  delete newgrid;
}

void gameoflifeLoop() {
  if (golNew) {
    //Display a simple splash screen
    tft.fillScreen(TFT_BLACK);
    //tft.setTextSize(2);
    tft.setFreeFont(FSS24);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Game", 120, 60, GFXFF);
    tft.drawString("of", 120, 120, GFXFF);
    tft.drawString("Life", 120, 180, GFXFF);
    tft.setTextDatum(TL_DATUM);
  
    delay(1000);
  
    tft.fillScreen(TFT_BLACK);
    initGrid();
    genCount = 0;
    drawGrid();

    golNew = false;
  }

  //Compute generations
//  for (int gen = 0; gen < genCount; gen++)
//  {
//    computeCA();
//    drawGrid();
//    delay(GEN_DELAY);
//    nextGrid();
//  }


  computeCA();
  drawGrid();
  nextGrid();

  genCount += 1;
  if (genCount > NUMGEN) {
    golNew = true;
  }
}

void nextGrid() {
  for (int16_t x = 1; x < GRIDX-1; x++) {
    for (int16_t y = 1; y < GRIDY-1; y++) {
      grid[x+GRIDX*y] = newgrid[x+GRIDX*y];
    }
  }
}

//Draws the grid on the display
void drawGrid(void) {

  uint16_t color = TFT_WHITE;
  for (int16_t x = 1; x < GRIDX - 1; x++) {
    for (int16_t y = 1; y < GRIDY - 1; y++) {
      if ((grid[x+GRIDX*y]) != (newgrid[x+GRIDX*y])) {
        if (newgrid[x+GRIDX*y] == 1) {
          color = 0xFFFF; //random(0xFFFF);
        }
        else {
          color = 0;
        }
        // Expand drawn grid by CELLXY scale factor
        tft.fillRect(CELLXY * x, CELLXY * y, CELLXY, CELLXY, color);
      }
    }
  }
}

//Initialise Grid
void initGrid(void) {
  for (int16_t x = 0; x < GRIDX; x++) {
    for (int16_t y = 0; y < GRIDY; y++) {
      newgrid[x+GRIDX*y] = 0;

      if (x == 0 || x == GRIDX - 1 || y == 0 || y == GRIDY - 1) {
        grid[x+GRIDX*y] = 0;
      }
      else {
        if (random(3) == 1)
          grid[x+GRIDX*y] = 1;
        else
          grid[x+GRIDX*y] = 0;
      }

    }
  }
}

//Compute the CA. Basically everything related to CA starts here
void computeCA() {
  for (int16_t x = 1; x < GRIDX; x++) {
    for (int16_t y = 1; y < GRIDY; y++) {
      int neighbors = getNumberOfNeighbors(x, y);
      if (grid[x+GRIDX*y] == 1 && (neighbors == 2 || neighbors == 3 ))
      {
        newgrid[x+GRIDX*y] = 1;
      }
      else if (grid[x+GRIDX*y] == 1)  newgrid[x+GRIDX*y] = 0;
      if (grid[x+GRIDX*y] == 0 && (neighbors == 3))
      {
        newgrid[x+GRIDX*y] = 1;
      }
      else if (grid[x+GRIDX*y] == 0) newgrid[x+GRIDX*y] = 0;
    }
  }
}

// Check the Moore neighborhood
int getNumberOfNeighbors(int x, int y) {
  return grid[(x - 1)+GRIDX*y] 
    + grid[(x - 1)+GRIDX*(y - 1)]
    + grid[x+GRIDX*(y - 1)] 
    + grid[(x + 1)+GRIDX*(y - 1)] 
    + grid[(x + 1)+GRIDX*y] 
    + grid[(x + 1)+GRIDX*(y + 1)] 
    + grid[x+GRIDX*(y + 1)] 
    + grid[(x - 1)+GRIDX*(y + 1)];
}

/*
   The MIT License (MIT)

   Copyright (c) 2016 RuntimeProjects.com

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
