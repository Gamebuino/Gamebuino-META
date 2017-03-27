#include <Gamebuino-Meta.h>

File file;

#define FILES_PER_PAGE 10
#define MAX_NAME_LEN 30

char pageFiles[FILES_PER_PAGE][MAX_NAME_LEN];
uint16_t pageFileColors[FILES_PER_PAGE];
char path[512];
int8_t cursorPos = 0;
int8_t cursorPos_max = 0;
uint32_t page_offset = 0;

bool loadPage(uint32_t offset) {
  SerialUSB.print("Loading page ");
  SerialUSB.println(offset);
  file.rewindDirectory();
  File entry;
  uint32_t i = 0;
  if (strlen(path) != 0) {
      i++; // we have the sneaky ".."-path
  }
  while (i < offset) {
    if (!file.openNextFile()) {
      SerialUSB.println("offset too far, nothing to display");
      SerialUSB.println("======\n");
      return false;
    }
    i++;
  }
  SerialUSB.println("found offset");
  i = 0;
  if (strlen(path) != 0 && offset == 0) {
    i = 1;
    strcpy(pageFiles[0], "/..");
    pageFileColors[0] = GREEN;
  }
  for (; i < FILES_PER_PAGE; i++) {
    entry = file.openNextFile();
    if (!entry) {
      if (i == 0) {
        SerialUSB.println("======\n");
        return false; // page failed, as there is nothing to display
      }
      break;
    }

    entry.getName(pageFiles[i], MAX_NAME_LEN);
    SerialUSB.println(pageFiles[i]);
    uint16_t color = WHITE;
    //gray out system entry
    if (entry.isSystem()) {
      color = BROWN;
    } else if (entry.isDirectory()) {
      color = GREEN;
      //dirty way to add a slash in front of directories names
      entry.getName(pageFiles[i] + 1, MAX_NAME_LEN - 1);
      pageFiles[i][0] = '/';
    } else if (!(strstr(pageFiles[i], ".BIN") || strstr(pageFiles[i], ".bin"))) {
      //gray out non .bin files
      color = BROWN;
    } else if ((strstr(pageFiles[i], "loader.bin"))){
      //gray out LOADER.BIN
      color = BROWN;
    }
    pageFileColors[i] = color;
    entry.close();
  }
  cursorPos_max = i;
  SerialUSB.print("Files on page: ");
  SerialUSB.println(i);
  SerialUSB.println("======\n");
  return true;
}

File getEntry(uint32_t offset) {
  if (strlen(path) != 0) {
      offset--; // we have the sneaky ".." path
  }
  file.rewindDirectory();
  uint32_t i = 0;
  while (i < offset) {
    file.openNextFile();
    i++;
  }
  return file.openNextFile();
}

void handlePress() {
  if (strlen(path) != 0 && page_offset == 0 && cursorPos == 0) {
    uint16_t i = strlen(path) - 1;
    while(i > 0) {
      path[i] = '\0';
      i--;
      if (path[i] == '/') {
          break;
      }
    }
    if (i == 0) {
      // we are back at root which is kinda a special case...
      path[0] = '\0';
      file = SD.open("/");
    } else {
      file = SD.open(path);
    }
    page_offset = 0;
    cursorPos = 0;
    loadPage(0);
    return;
  }
  File entry = getEntry(page_offset + cursorPos);
  if (!entry) {
    return;
  }
  if (entry.isDirectory()) {
    
    // we need to add on to the path
    uint16_t l = strlen(path);
    entry.getName(path + l, 512 - l);
    l = strlen(path);
    path[l] = '/';
    path[l+1] = '\0';
    
    file = SD.open(path);
    
    page_offset = 0;
    cursorPos = 0;
    loadPage(0);
    return;
  }
  if (entry.isSystem()){
    return;
  }
  uint16_t l = strlen(path);
  entry.getName(path + l, 512 - l);
  
  SerialUSB.println("Loading file...");
  SerialUSB.println(path);
  
  gb.display.fillScreen(BLACK);
  gb.display.setColor(WHITE);
  gb.display.println("\n\n\nLOADING");
  uint16_t i = 0;
  while (strlen(path + i) > 20) {
    gb.display.println(path + i);
    i += 20;
  }
  gb.display.println(path + i);
  // update the screen
  gb.tft.drawImage(0, 0, gb.display, gb.tft.width(), gb.tft.height());
  //while(1);
  
  ((void(*)(const char* filename))(*((uint32_t*)0x3FF8)))(path);
}

void setup() {
  gb.begin();
  SerialUSB.begin(9600);
  //	while(!SerialUSB);
  path[0] = '\0';
  file = SD.open("/");
  loadPage(0);
}

void loop() {
  if (gb.update()) {
    // first check for button presses
    if (gb.buttons.pressed(BTN_UP)) {
      cursorPos--;
      if (cursorPos < 0) {
        if (page_offset > 0) {
          page_offset -= FILES_PER_PAGE;
          loadPage(page_offset);
          cursorPos = FILES_PER_PAGE - 1;
        } else {
          cursorPos = 0;
        }
      }
    }
    if (gb.buttons.pressed(BTN_DOWN)) {
      cursorPos++;
      if (cursorPos >= cursorPos_max) {
        page_offset += FILES_PER_PAGE;
        if (loadPage(page_offset)) {
          cursorPos = 0;
        } else {
          // this page is actually empty so just stick to the current one
          cursorPos--;
          page_offset -= FILES_PER_PAGE;
        }
      }
    }

    if (gb.buttons.pressed(BTN_A)) {
      handlePress();
    }

    // draw the screen
    gb.display.fillScreen(BLACK);
    gb.display.setColor(WHITE);
    gb.display.textWrap = false;
    for (uint8_t i = 0; i < cursorPos_max; i++) {
      gb.display.setColor(pageFileColors[i]);
      gb.display.cursorX = 5;
      gb.display.cursorY = 2 + 6 * i;
      gb.display.print(pageFiles[i]);
    }
    gb.display.setColor(WHITE);
    gb.display.cursorX = 1;
    gb.display.cursorY = 2 + 6 * cursorPos;
    if ((gb.frameCount) % 10 < 5) {
      gb.display.print(">");
    }
  }
}
