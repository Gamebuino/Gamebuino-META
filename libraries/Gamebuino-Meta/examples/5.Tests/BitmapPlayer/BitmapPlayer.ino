#include <Gamebuino-Meta.h>

const int numFiles = 17;
int currentFile = 0;

File root;
File entry;
char path[50];
int popupTimer = 0;
int nextTimer = 0;

Image img;
void setup() {
  gb.begin();

  root = SD.open("/BitmapPlayer");
  if (!loadNextBitmap()) {
    while (1) {
      if (gb.update()) {
        gb.display.println("NO BITMAP FOUND");
      }
    }
  }
}

boolean loadNextBitmap() {
  do {
    File entry = root.openNextFile();
    if (!entry) {
      entry.close();
      root.rewindDirectory();
      return false;
    }
    entry.getName(path, 50);
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
  if (gb.update()) {
    //if multiple frames
    if (img.frames > 1) { 
      //load after the last one
      if (img.frame == (img.frames - 1)) {
        loadNextBitmap();
      }
      //if single frame, load after a while
    } else {
      nextTimer--;
      if (nextTimer <= 0) {
      loadNextBitmap();
      }
    }

    if (gb.buttons.repeat(BUTTON_A, 10)) {
      loadNextBitmap();
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
  }
}