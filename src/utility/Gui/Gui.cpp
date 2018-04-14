/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2018

This is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL)
as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License (LGPL) for more details.

You should have received a copy of the GNU Lesser General Public
License (LGPL) along with the library.
If not, see <http://www.gnu.org/licenses/>.

Authors:
 - Sorunome
*/

#include "Gui.h"
#include <Gamebuino-Meta.h>

namespace Gamebuino_Meta {

///////////////////////
// GUI keyboard start
///////////////////////


const char keyboardLayoutPage0[] = "1234567890azertyuiop+-=qsdfghjklm*/.<>wxcvbn_?';,AB";
const char keyboardLayoutPage1[] = "1234567890AZERTYUIOP+-=QSDFGHJKLM*/.<>WXCVBN_?';,ab";
const char* keyboardLayout[] = {
	keyboardLayoutPage0,
	keyboardLayoutPage1,
};
const uint8_t keyboardLayoutPages = 2;

const uint8_t keyboardYOffset = 23;

void keyboardDrawKey(uint8_t x, uint8_t y, char c) {
	gb.display.setColor(DARKGRAY);
	gb.display.fillRect(x+1, y+1, 5, 7);
	gb.display.setColor(WHITE);
	gb.display.setCursor(x+2, y+2);
	gb.display.print(c);
}

void keyboardDrawLayout(const char* layout) {
	for (uint8_t y = 0; y < 4; y++) {
		for (uint8_t x = 0; x < 13; x++) {
			if (y == 0 && x >= 10) {
				// we skip the backspace key
				break;
			}
			keyboardDrawKey(x*6, y*8 + keyboardYOffset, *(layout++));
		}
	}
	// last we draw the new layout switch
	gb.display.setColor(DARKGRAY);
	gb.display.fillRect(7, keyboardYOffset + 1 + 4*8, 11, 7);
	gb.display.setColor(GRAY);
	gb.display.setCursor(9, keyboardYOffset + 2 + 4*8);
	gb.display.print(layout);
}

void keyboardDrawBackspace() {
	const uint8_t arrow[] = {
		8, 5,
		0x20, 0x40, 0xFF, 0x40, 0x20,
	};
	gb.display.setColor(DARKGRAY);
	gb.display.fillRect(6*10+1, keyboardYOffset + 1, 17, 7);
	gb.display.setColor(GRAY);
	gb.display.drawBitmap(6*10+9, keyboardYOffset+2, arrow);
}

void keyboardDrawSwitch() {
	const uint8_t menu[] = {
		8, 5,
		0xB8, 0x00, 0xB8, 0x00, 0xB8,
	};
	gb.display.setColor(DARKGRAY);
	gb.display.fillRect(1, keyboardYOffset + 1 + 4*8, 17, 7);
	gb.display.setColor(GRAY);
	gb.display.drawBitmap(2, keyboardYOffset + 2 + 4*8, menu);
}

void keyboardDrawSelect() {
	const uint8_t checkmark[] = {
		8, 4,
		0x08, 0x10, 0xA0, 0x40,
	};
	gb.display.setColor(DARKGRAY);
	gb.display.fillRect(6*10+1, keyboardYOffset + 1 + 4*8, 17, 7);
	gb.display.setColor(LIGHTGREEN);
	gb.display.drawBitmap(6*10+7, keyboardYOffset + 3 + 4*8, checkmark);
}

void keyboardDrawSpace() {
	gb.display.setColor(DARKGRAY);
	gb.display.fillRect(19, keyboardYOffset + 1 + 4*8, 41, 7);
}

void keyboardDrawCursorReal(uint8_t x, int8_t y) {
	if (y == 0 && x >= 10) {
		// blinking backspace
		gb.display.drawRect(10*6, keyboardYOffset, 19, 9);
		return;
	}
	if (y == 4) {
		// lower row
		if (x < 3) {
			// switch
			gb.display.drawRect(0, keyboardYOffset + 4*8, 19, 9);
			return;
		}
		if (x < 10) {
			// space
			gb.display.drawRect(3*6, keyboardYOffset + 4*8, 43, 9);
			return;
		}
		// select
		gb.display.drawRect(10*6, keyboardYOffset + 4*8, 19, 9);
		return;
	}
	gb.display.drawRect(x*6, y*8 + keyboardYOffset, 7, 9);
}

void keyboardDrawCursor(int8_t x, int8_t y) {
	gb.display.setColor((gb.frameCount % 8) >= 4 ? BROWN : BLACK);
	keyboardDrawCursorReal(x, y);
}

void keyboardEraseCursor(int8_t x, int8_t y) {
	gb.display.setColor(BLACK);
	keyboardDrawCursorReal(x, y);
}

char keyboardGetChar(int8_t x, int8_t y, const char* layout) {
	if (y == 4) {
		if (x < 10 && x >= 3) {
			return ' ';
		}
		return 0;
	}
	if (y == 0 && x >= 10) {
		return 0;
	}
	if (y > 0) { // skip the backspace button
		x -= 3;
	}
	return layout[13*y + x];
}

void Gui::keyboard(const char* title, char* text, uint8_t length) {
	bool reInitAsIndexed = false;
	if (gb.display.width() == 160) {
		reInitAsIndexed = true;
		gb.display.init(80, 64, ColorMode::rgb565);
	}
	gb.display.fill(BLACK);
	gb.display.setColor(DARKGRAY);
	gb.display.fillRect(0, 0, gb.display.width(), 7);
	gb.display.setCursor(1, 1);
	gb.display.setColor(WHITE);
	gb.display.print(title);
	
	
	keyboardDrawBackspace();
	keyboardDrawSwitch();
	keyboardDrawSpace();
	keyboardDrawSelect();
	
	uint8_t curLayout = 0;
	
	keyboardDrawLayout(keyboardLayout[curLayout]);
	
	int8_t cursorX = 6;
	int8_t cursorY = 2;
	int8_t activeChar = 0;
	while(1) {
		while(!gb.update());
		// update cursor movement
		int8_t cursorXPrev = cursorX;
		int8_t cursorYPrev = cursorY;
		keyboardEraseCursor(cursorX, cursorY);
		if (gb.buttons.repeat(BUTTON_LEFT, 4)) {
			if ((cursorY == 0 || cursorY == 4) && cursorX >= 10) {
				cursorX = 9;
			} else if (cursorY == 4 && cursorX >= 3) {
				cursorX = 2;
			} else {
				cursorX--;
			}
		}
		if (gb.buttons.repeat(BUTTON_RIGHT, 4)) {
			if (cursorY == 4 && cursorX < 3) {
				cursorX = 3;
			} else if (cursorY == 4 && cursorX < 10) {
				cursorX = 10;
			} else {
				cursorX++;
			}
		}
		cursorX = min(12, max(0, cursorX));
		cursorY = min(4, max(0, cursorY - gb.buttons.repeat(BUTTON_UP, 4) + gb.buttons.repeat(BUTTON_DOWN, 4)));
		keyboardDrawCursor(cursorX, cursorY);
		if (cursorX != cursorXPrev || cursorY != cursorYPrev) {
			gb.sound.playTick();
		}
		
		// check for other button presses
		bool backspace = gb.buttons.pressed(BUTTON_B);
		bool switchLayout = gb.buttons.pressed(BUTTON_MENU);
		if (gb.buttons.pressed(BUTTON_A)) {
			char c = keyboardGetChar(cursorX, cursorY, keyboardLayout[curLayout]);
			if (!c) {
				// other handling
				if (cursorY == 0) {
					backspace = true;
				} else if (cursorY == 4 && cursorX < 3) {
					switchLayout = true;
				} else {
					// we are done!
					gb.sound.playOK();
					break;
				}
			} else if (activeChar < length) {
				text[activeChar++] = c;
				text[activeChar] = '\0';
				gb.sound.playOK();
			}
		}
		if (backspace) {
			if (activeChar > 0) {
				text[--activeChar] = '\0';
				gb.sound.playCancel();
			}
		}
		if (switchLayout) {
			if (++curLayout >= keyboardLayoutPages) {
				curLayout = 0;
			}
			keyboardDrawLayout(keyboardLayout[curLayout]);
			gb.sound.playOK();
		}
		
		// render drawing text
		gb.display.setColor(BLACK);
		gb.display.fillRect(0, 7, gb.display.width(), 15);
		gb.display.setColor(activeChar?WHITE:DARKGRAY);
		gb.display.setCursor(1, 13);
		gb.display.print(text);
		
		gb.display.setColor(WHITE);
		gb.display.drawFastHLine(activeChar*4 + 1, 19, 3);
	}
	if (reInitAsIndexed) {
		gb.display.init(160, 128, ColorMode::index);
	}
}

void Gui::keyboard(const MultiLang* title, char* text, uint8_t length, uint8_t numLang) {
	keyboard(gb.language.get(title, numLang), text, length);
}

///////////////////////
// GUI keyboard end
///////////////////////


}; // namespace Gamebuino_Meta
