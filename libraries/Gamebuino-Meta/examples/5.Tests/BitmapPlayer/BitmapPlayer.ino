#include <Gamebuino-Meta.h>

const int numFiles = 17;
int currentFile = 0;

File root;
File entry;
char path[50];

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
  gb.tft.print("LOADING IMAGE");
  img.init(path, 0);
  return true;
}

void loop() {
  if (gb.update()) {

    if ((gb.frameCount % 50) == 0) {
      loadNextBitmap();
    }
    gb.display.drawImage(0, 0, img);
    if ((gb.frameCount % 50) <= 20) {
      //show file name
      gb.display.setColor(WHITE,BLACK);
      gb.display.cursorX = 0;
      gb.display.cursorY = 0;
      gb.display.println(path);
    }
  }
}