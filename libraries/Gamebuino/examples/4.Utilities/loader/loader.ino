#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;
extern SdFat SD;
File file;

#define FILES_PER_PAGE 10
#define MAX_NAME_LEN 18

char pageFiles[FILES_PER_PAGE][MAX_NAME_LEN];
uint16_t pageFileColors[FILES_PER_PAGE];
char buf[512];
int8_t cursorPos = 0;
int8_t cursorPos_max = 0;
uint32_t page_offset = 0;

bool loadPage(uint32_t offset) {
	SerialUSB.print("Loading page ");
	SerialUSB.println(offset);
	file.rewindDirectory();
	File entry;
	uint32_t i = 0;
	while(i < offset) {
		if(!file.openNextFile()) {
			SerialUSB.println("offset too far, nothing to display");
			SerialUSB.println("======\n");
			return false;
		}
		i++;
	}
	SerialUSB.println("found offset");
	
	for(i = 0; i < FILES_PER_PAGE; i++) {
		entry = file.openNextFile();
		if(!entry) {
			if (i == 0) {
				SerialUSB.println("======\n");
				return false; // page failed, as there is nothing to display
			}
			break;
		}
		
		entry.getName(pageFiles[i],MAX_NAME_LEN);
		SerialUSB.println(pageFiles[i]);
		uint16_t color = BLACK;
		if (entry.isSystem()) {
			color = RED;
		} else if (entry.isDirectory()) {
			color = 0x0603; // dark-ish green
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
	file.rewindDirectory();
	uint32_t i = 0;
	while(i < offset) {
		file.openNextFile();
		i++;
	}
	return file.openNextFile();
}

void handlePress() {
	File entry = getEntry(page_offset + cursorPos);
	if (!entry) {
		return;
	}
//	entry.getName(buf,512);
	if (entry.isDirectory()) {
		gb.display.println("force-loading uforace");
		// update the screen
		gb.tft.drawImage(0, 0, gb.display, gb.tft.width(), gb.tft.height());
		((void(*)(const char* filename))(*((uint32_t*)0x3FF8)))("uforace.ino.bin");
		file = entry;
		page_offset = 0;
		cursorPos = 0;
		loadPage(0);
		return;
	}
	entry.getName(buf,512);
	SerialUSB.println("Loading file...");
	SerialUSB.println(buf);
//	((void(*)())(*((uint32_t*)0x3FF4)))();
	gb.display.println("Loading file...");
	gb.display.print(buf);
	// update the screen
	gb.tft.drawImage(0, 0, gb.display, gb.tft.width(), gb.tft.height());
//	((void(*)(const char* filename))(*((uint32_t*)0x3FF8)))("uforace.ino.bin");
	((void(*)(const char* filename))(*((uint32_t*)0x3FF8)))(buf);
}

void setup() {
	gb.begin();
	SerialUSB.begin(9600);
//	while(!SerialUSB);
	
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
		for (uint8_t i = 0; i < cursorPos_max; i++) {
			gb.display.setColor(pageFileColors[i]);
			gb.display.cursorX = 5;
			gb.display.cursorY = 2 + 6*i;
			gb.display.print(pageFiles[i]);
		}
		gb.display.setColor(BLUE);
		gb.display.cursorX = 1;
		gb.display.cursorY = 2 + 6*cursorPos;
		gb.display.print(">");
	}
}
