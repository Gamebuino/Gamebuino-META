#include <Gamebuino-Meta.h>

const int numFiles = 17;
int currentFile = 0;

File root;
File entry;
char path[50];
int popupTimer = 0;
int slideShowTimer = 0;
int nextTimer = 0;
bool slideShow = true;

Image img;
void setup() {
  gb.begin();

  root = SD.open("SAMPLES");
  if (!loadNextBitmap()) {
    while (1) {
      while(!gb.update());
      gb.display.clear();
      gb.display.println("NO BITMAP FOUND");
    }
  }
}

boolean loadNextBitmap() {
  strcpy(path, "SAMPLES/");
  uint8_t i = strlen(path);
  do {
    File entry = root.openNextFile();
    if (!entry) {
      entry.close();
      root.rewindDirectory();
      return false;
    }
    entry.getName(path + i, 50 - i);
    entry.close();
    gb.display.println(path);
  } while (!(strstr(path, ".BMP") || strstr(path, ".bmp")));
  gb.tft.cursorX = 0;
  gb.tft.cursorY = 0;
  gb.tft.setFontSize(2);
  gb.tft.setColor(WHITE, BLACK);
  gb.tft.print(path);
  gb.tft.setFontSize(1);
  img.init(80, 64, path);
  popupTimer = 20;
  nextTimer = 50;
  return true;
}

void loop() {
  while(!gb.update());
  // clear the previous screen
  gb.display.clear();
  
  //if multiple frames
  if (img.frames > 1) { 
    //load after the last one
    if (img.frame == (img.frames - 1)) {
      loadNextBitmap();
    }
    //if single frame, load after a while
  } else {
    nextTimer--;
    if ((nextTimer <= 0 && slideShow)) {
      loadNextBitmap();
    }
  }
  if (gb.buttons.repeat(BUTTON_A, 10)) {
    loadNextBitmap();
  }
  if (gb.buttons.pressed(BUTTON_B)) {
    slideShow = 1 - slideShow;
    slideShowTimer = 20;  
  }

  gb.display.drawImage(0, 0, img);
  if (popupTimer > 0) {
    popupTimer--;
    //show file name
    gb.display.setColor(WHITE, BLACK);
    gb.display.cursorX = 0;
    gb.display.cursorY = 0;
    gb.display.println(path);

  }
  if (slideShowTimer > 0) {
    slideShowTimer--;
    gb.display.setColor(WHITE, BLACK);
    gb.display.cursorX = 0;
    gb.display.cursorY = 58;
    if (slideShow) {
      gb.display.println(" Slide show enabled ");
    } else {
      gb.display.println("Slide show disabled ");
    }      
  }
}
