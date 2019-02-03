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

extern Gamebuino* gbptr;

///////////////////////
// GUI keyboard start
///////////////////////

extern const char* keyboardLayout[];
const uint8_t keyboardLayoutPages = 2;

const uint8_t keyboardYOffset = 23;

void keyboardDrawKey(uint8_t x, uint8_t y, char c) {
	gbptr->display.setColor(DARKGRAY);
	gbptr->display.fillRect(x+1, y+1, 5, 7);
	gbptr->display.setColor(WHITE);
	gbptr->display.setCursor(x+2, y+2);
	gbptr->display.print(c);
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
	gbptr->display.setColor(DARKGRAY);
	gbptr->display.fillRect(7, keyboardYOffset + 1 + 4*8, 11, 7);
	gbptr->display.setColor(GRAY);
	gbptr->display.setCursor(9, keyboardYOffset + 2 + 4*8);
	gbptr->display.print(layout);
}

void keyboardDrawBackspace() {
	const uint8_t arrow[] = {
		8, 5,
		0x20, 0x40, 0xFF, 0x40, 0x20,
	};
	gbptr->display.setColor(DARKGRAY);
	gbptr->display.fillRect(6*10+1, keyboardYOffset + 1, 17, 7);
	gbptr->display.setColor(GRAY);
	gbptr->display.drawBitmap(6*10+9, keyboardYOffset+2, arrow);
}

void keyboardDrawSwitch() {
	const uint8_t menu[] = {
		8, 5,
		0xB8, 0x00, 0xB8, 0x00, 0xB8,
	};
	gbptr->display.setColor(DARKGRAY);
	gbptr->display.fillRect(1, keyboardYOffset + 1 + 4*8, 17, 7);
	gbptr->display.setColor(GRAY);
	gbptr->display.drawBitmap(2, keyboardYOffset + 2 + 4*8, menu);
}

void keyboardDrawSelect() {
	const uint8_t checkmark[] = {
		8, 4,
		0x08, 0x10, 0xA0, 0x40,
	};
	gbptr->display.setColor(DARKGRAY);
	gbptr->display.fillRect(6*10+1, keyboardYOffset + 1 + 4*8, 17, 7);
	gbptr->display.setColor(LIGHTGREEN);
	gbptr->display.drawBitmap(6*10+7, keyboardYOffset + 3 + 4*8, checkmark);
}

void keyboardDrawSpace() {
	gbptr->display.setColor(DARKGRAY);
	gbptr->display.fillRect(19, keyboardYOffset + 1 + 4*8, 41, 7);
}

void keyboardDrawCursorReal(uint8_t x, int8_t y) {
	if (y == 0 && x >= 10) {
		// blinking backspace
		gbptr->display.drawRect(10*6, keyboardYOffset, 19, 9);
		return;
	}
	if (y == 4) {
		// lower row
		if (x < 3) {
			// switch
			gbptr->display.drawRect(0, keyboardYOffset + 4*8, 19, 9);
			return;
		}
		if (x < 10) {
			// space
			gbptr->display.drawRect(3*6, keyboardYOffset + 4*8, 43, 9);
			return;
		}
		// select
		gbptr->display.drawRect(10*6, keyboardYOffset + 4*8, 19, 9);
		return;
	}
	gbptr->display.drawRect(x*6, y*8 + keyboardYOffset, 7, 9);
}

void keyboardDrawCursor(int8_t x, int8_t y) {
	gbptr->display.setColor((gbptr->frameCount % 8) >= 4 ? BROWN : BLACK);
	keyboardDrawCursorReal(x, y);
}

void keyboardEraseCursor(int8_t x, int8_t y) {
	gbptr->display.setColor(BLACK);
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
	if (gbptr->display.width() == 160) {
		reInitAsIndexed = true;
		gbptr->display.init(80, 64, ColorMode::rgb565);
	}
	uint8_t fontSizeBak = gbptr->display.fontSize;
	gbptr->display.setFontSize(1);
	
	gbptr->display.fill(BLACK);
	gbptr->display.setColor(DARKGRAY);
	gbptr->display.fillRect(0, 0, gbptr->display.width(), 7);
	gbptr->display.setCursor(1, 1);
	gbptr->display.setColor(WHITE);
	gbptr->display.print(title);
	
	
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
		while(!gbptr->update());
		// update cursor movement
		int8_t cursorXPrev = cursorX;
		int8_t cursorYPrev = cursorY;
		keyboardEraseCursor(cursorX, cursorY);
		// cursorX movement
		if (gbptr->buttons.repeat(BUTTON_LEFT, 4)) {
			if ((cursorY == 0 || cursorY == 4) && cursorX >= 10) {
				cursorX = 9;
			} else if (cursorY == 4 && cursorX >= 3) {
				cursorX = 2;
			} else if (cursorX == 0 || (cursorX <= 2 && cursorY == 4)) {
				cursorX = 12;  // Wrap around
			} else {
				cursorX--;
			}
		}
		if (gbptr->buttons.repeat(BUTTON_RIGHT, 4)) {
			if (cursorY == 4 && cursorX < 3) {
				cursorX = 3;
			} else if (cursorY == 4 && cursorX < 10) {
				cursorX = 10;
			} else if (cursorX == 12 || (cursorX >=10 && (cursorY == 0 || cursorY == 4))) {
				cursorX = 0;  // Wrap around
			} else {
				cursorX++;
			}
		}
		// cursorY movement
		cursorY += gbptr->buttons.repeat(BUTTON_DOWN, 4) - gbptr->buttons.repeat(BUTTON_UP, 4);
		if (cursorY > 4) cursorY = 0;
		else if (cursorY < 0) cursorY = 4;

		keyboardDrawCursor(cursorX, cursorY);
		if (cursorX != cursorXPrev || cursorY != cursorYPrev) {
			gbptr->sound.playTick();
		}
		
		// check for other button presses
		bool backspace = gbptr->buttons.released(BUTTON_B);
		bool switchLayout = gbptr->buttons.released(BUTTON_MENU);
		if (gbptr->buttons.released(BUTTON_A)) {
			char c = keyboardGetChar(cursorX, cursorY, keyboardLayout[curLayout]);
			if (!c) {
				// other handling
				if (cursorY == 0) {
					backspace = true;
				} else if (cursorY == 4 && cursorX < 3) {
					switchLayout = true;
				} else {
					// we are done!
					gbptr->sound.playOK();
					break;
				}
			} else if (activeChar < length) {
				text[activeChar++] = c;
				text[activeChar] = '\0';
				gbptr->sound.playOK();
			}
		}
		if (backspace) {
			if (activeChar > 0) {
				text[--activeChar] = '\0';
				gbptr->sound.playCancel();
			}
		}
		if (switchLayout) {
			if (++curLayout >= keyboardLayoutPages) {
				curLayout = 0;
			}
			keyboardDrawLayout(keyboardLayout[curLayout]);
			gbptr->sound.playOK();
		}
		
		// render drawing text
		gbptr->display.setColor(BLACK);
		gbptr->display.fillRect(0, 7, gbptr->display.width(), 15);
		gbptr->display.setColor(activeChar?WHITE:DARKGRAY);
		gbptr->display.setCursor(1, 13);
		gbptr->display.print(text);
		
		gbptr->display.setColor(WHITE);
		gbptr->display.drawFastHLine(activeChar*4 + 1, 19, 3);
	}
	if (reInitAsIndexed) {
		gbptr->display.init(160, 128, ColorMode::index);
	}
	gbptr->display.setFontSize(fontSizeBak);
}

void Gui::keyboard(const MultiLang* title, char* text, uint8_t length, uint8_t numLang) {
	keyboard(gbptr->language.get(title, numLang), text, length);
}

///////////////////////
// GUI keyboard end
///////////////////////


///////////////////////
// GUI menu start
///////////////////////
const uint8_t menuYOffset = 9;
void menuDrawBox(const char* text, uint8_t i, int32_t y, uint8_t numLang) {
	y += i*8 + menuYOffset;
	if (y < 0 || y > 64) {
		return;
	}
	gbptr->display.setColor(DARKGRAY);
	gbptr->display.fillRect(1, y+1, gbptr->display.width()-2, 7);
	gbptr->display.setColor(WHITE);
	gbptr->display.setCursor(2, y+2);
	if (numLang == 0) {
		gbptr->display.print(text);
	} else {
		gbptr->display.print(gbptr->language.get((const MultiLang*)text, numLang));
	}
}

void menuDrawCursor(uint8_t i, int32_t y) {
	if ((gbptr->frameCount % 8) < 4) {
		return;
	}
	y += i*8 + menuYOffset;
	gbptr->display.setColor(BROWN);
	gbptr->display.drawRect(0, y, gbptr->display.width(), 9);
}

uint8_t Gui::menu(const char* title, const char** items, uint8_t length, uint8_t numLang) {
	bool reInitAsIndexed = false;
	if (gbptr->display.width() == 160) {
		reInitAsIndexed = true;
		gbptr->display.init(80, 64, ColorMode::rgb565);
	}
	uint8_t fontSizeBak = gbptr->display.fontSize;
	
	uint8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	
	while(1) {
		while(!gbptr->update());
		gbptr->display.clear();
		gbptr->display.setFontSize(1);
		
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}
		
		for (uint8_t i = 0; i < length; i++) {
			menuDrawBox(items[i], i, cameraY_actual, numLang);
		}
		
		menuDrawCursor(cursor, cameraY_actual);
		
		// last draw the top entry thing
		gbptr->display.setColor(DARKGRAY);
		gbptr->display.fillRect(0, 0, gbptr->display.width(), 7);
		gbptr->display.setColor(WHITE);
		gbptr->display.setCursor(1, 1);
		gbptr->display.print(title);
		gbptr->display.setColor(BLACK);
		gbptr->display.drawFastHLine(0, 7, gbptr->display.width());
		
		if (gbptr->buttons.released(BUTTON_A)) {
			gbptr->sound.playOK();
			break;
		}
		
		if (gbptr->buttons.repeat(BUTTON_UP, 4)) {
			if (cursor == 0) {
				cursor = length - 1;
				if (length > 6) {
					cameraY = -(cursor-5)*8;
				}
			} else {
				cursor--;
				if (cursor > 0 && (cursor*8 + cameraY + menuYOffset) < 14) {
					cameraY += 8;
				}
			}
			gbptr->sound.playTick();
		}
		
		if (gbptr->buttons.repeat(BUTTON_DOWN, 4)) {
			cursor++;
			if ((cursor*8 + cameraY + menuYOffset) > 54) {
				cameraY -= 8;
			}
			if (cursor >= length) {
				cursor = 0;
				cameraY = 0;
			}
			gbptr->sound.playTick();
		}
	}
	if (reInitAsIndexed) {
		gbptr->display.init(160, 128, ColorMode::index);
	}
	gbptr->display.setFontSize(fontSizeBak);
	return cursor;
}

uint8_t Gui::menu(const MultiLang* title, const MultiLang** items, uint8_t length, uint8_t numLang) {
	return menu(gbptr->language.get(title, numLang), (const char**)items, length, numLang);
}

///////////////////////
// GUI menu end
///////////////////////


///////////////////////
// GUI popup start
///////////////////////

#if GUI_ENABLE_POPUP

const char* popupText;
uint8_t popupTimeLeft;
uint8_t popupTotalTime;

void Gui::popup(const char* text, uint8_t duration) {
	popupText = text;
	popupTotalTime = popupTimeLeft = duration + 16;
}

void Gui::updatePopup() {
	if (!popupTimeLeft) {
		return;
	}
	uint8_t scale = gbptr->display.fontSize;
	uint8_t yOffset = 0;
	if (popupTimeLeft >= popupTotalTime-8) {
		yOffset = (8-(popupTotalTime - popupTimeLeft))*scale;
	}
	if (popupTimeLeft<8){
		yOffset = (8-popupTimeLeft)*scale;
	}
	
	gbptr->display.setColor(DARKGRAY);
	gbptr->display.fillRect(0, gbptr->display.height()-(7*scale)+yOffset, gbptr->display.width(), 7*scale);
	gbptr->display.setColor(BROWN);
	gbptr->display.fillRect(0, gbptr->display.height()-(8*scale)+yOffset, gbptr->display.width(), scale);
	gbptr->display.setColor(WHITE);
	gbptr->display.setCursor(1, gbptr->display.height()-(6*scale)+yOffset);
	gbptr->display.print(popupText);
	
	popupTimeLeft--;
}

void Gui::popup(const MultiLang* text, uint8_t duration, uint8_t numLang) {
	popup(gbptr->language.get(text, numLang), duration);
}

#endif // GUI_ENABLE_POPUP

///////////////////////
// GUI popup end
///////////////////////
}; // namespace Gamebuino_Meta
