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

#include "MetaMode.h"
#include <Gamebuino-Meta.h>

namespace Gamebuino_Meta {

const uint8_t textMeta_w = 51;
const uint8_t textMeta_h = 15;
const uint8_t textMeta[] = {
    textMeta_w, textMeta_h, 
    0b11111111,0b11111110,0b00111111,0b11100011,0b11111110,0b00111111,0b11100000,
    0b11111111,0b11111110,0b00111111,0b11100011,0b11111110,0b00111111,0b11100000,
    0b11111111,0b11111110,0b00111111,0b11100011,0b11111110,0b00111111,0b11100000,
    0b11100011,0b10001110,0b00111000,0b00000000,0b01110000,0b00111000,0b11100000,
    0b11100011,0b10001110,0b00111000,0b00000000,0b01110000,0b00111000,0b11100000,
    0b11100011,0b10001110,0b00111000,0b00000000,0b01110000,0b00111000,0b11100000,
    0b11100011,0b10001110,0b00111111,0b11100000,0b01110000,0b00111111,0b11100000,
    0b11100011,0b10001110,0b00111111,0b11100000,0b01110000,0b00111111,0b11100000,
    0b11100011,0b10001110,0b00111111,0b11100000,0b01110000,0b00111111,0b11100000,
    0b11100011,0b10001110,0b00111000,0b00000000,0b01110000,0b00111000,0b11100000,
    0b11100011,0b10001110,0b00111000,0b00000000,0b01110000,0b00111000,0b11100000,
    0b11100011,0b10001110,0b00111000,0b00000000,0b01110000,0b00111000,0b11100000,
    0b11100011,0b10001110,0b00111111,0b11100000,0b01110000,0b00111000,0b11100000,
    0b11100011,0b10001110,0b00111111,0b11100000,0b01110000,0b00111000,0b11100000,
    0b11100011,0b10001110,0b00111111,0b11100000,0b01110000,0b00111000,0b11100000,
};

const uint8_t textMode_w = 36;
const uint8_t textMode_h = 12;
const uint8_t textMode[] = {
    textMode_w, textMode_h,
    0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
    0b01111111,0b11100111,0b11100111,0b10000111,0b11100000,
    0b01111111,0b11100111,0b11100111,0b10000111,0b11100000,
    0b01100110,0b01100110,0b01100110,0b01100110,0b00000000,
    0b01100110,0b01100110,0b01100110,0b01100110,0b00000000,
    0b01100110,0b01100110,0b01100110,0b01100111,0b11100000,
    0b01100110,0b01100110,0b01100110,0b01100111,0b11100000,
    0b01100110,0b01100110,0b01100110,0b01100110,0b00000000,
    0b01100110,0b01100110,0b01100110,0b01100110,0b00000000,
    0b01100110,0b01100111,0b11100111,0b10000111,0b11100000,
    0b01100110,0b01100111,0b11100111,0b10000111,0b11100000,
    0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
};

const uint8_t NUMBER_OF_COLORS_LINE_PATTERN = 8;
const Color linePattern[NUMBER_OF_COLORS_LINE_PATTERN] = {  // Color of the bottom and top bars
    WHITE, WHITE, WHITE, WHITE, ORANGE, ORANGE, ORANGE, ORANGE};

const Color rectsPattern[4] = {  // Colors of the sliding rectangles during launch animation
    WHITE, BEIGE, ORANGE, BROWN};

uint8_t currentPattern = 0;

bool animRunning = false;
int16_t animTimer = 0;
const uint8_t animRectTimeMax = 10;  // Time per sliding rect

const uint8_t animH_start = 18;  // Height of the sliding rects when the start in the center
const uint8_t animH_end = 2;     // Their height at the last frame
const uint8_t animInterval = 7;  // Frames between the start of 2 sliding rectangles

uint8_t loadingTimer = 0;
const uint8_t loadingTimeMax = 40;  // Time in frames you have to hold HOME and MENU

bool unhandledAnimRunning = false;
const uint8_t unhandledAnimTimeMax = 8;
uint8_t unhandledAnimTimer = 0;

bool MetaMode::isActive() {
	handled = true;
	return active;
};

bool MetaMode::isUsingHomeButton() {
	return usingHomeButton;
}

void MetaMode::update() {
	
	// ================== INPUTS ================== //
	if (gb.buttons.repeat(Button::home, 0) && gb.buttons.repeat(Button::menu, 0) && !active &&
	    !animRunning && !unhandledAnimRunning && canActivate) {
		loadingTimer++;
		if (loadingTimer >= loadingTimeMax / 2)
			usingHomeButton = true;  // From now on, releasing the home button doesn't open the Gamebuino menu
		
		if (loadingTimer == loadingTimeMax) {  // Once we fully loaded: launch animation, reset
			                                      // timer, and activate metaMode
			if (handled) {
				animTimer = 0;
				animRunning = true;
			} else {
				unhandledAnimRunning = true;
				unhandledAnimTimer = 0;
			}
		}
	} else if (gb.buttons.repeat(Button::home, 0) && gb.buttons.repeat(Button::menu, 0) &&
	           canDeactivate) {
		loadingTimer--;           // De-load (like loading, but backwards)
		if (loadingTimer <= loadingTimeMax / 2)
			usingHomeButton = true;  // From now on, releasing the home button doesn't open the Gamebuino menu
		
		if (loadingTimer == 0) {  // Once fully de-loaded, deactivate MetaMode
			active = false;
			canDeactivate = false;
			canActivate = false;  // The user must release then hold the HOME and MENU buttons again to re-activate the mode
		}
	} else if (gb.buttons.released(Button::home) || gb.buttons.released(Button::menu)) {
		canActivate = true;
		if (gb.buttons.released(Button::home))  // Only do this when releasing HOME
			usingHomeButton = false;  // MetaMode is no longer preventing the Gamebuino menu to open
		if (!active && !animRunning &&
		    !unhandledAnimRunning)  // If the user lets go while loading
			loadingTimer = 0;
		else if (canDeactivate)  // If the user lets go while de-loading
			loadingTimer = loadingTimeMax;
		else if (active || animRunning) 	// If the user lets go once the MetaMode is armed, it means that they can
											// now press the combination again to deactivate it
			canDeactivate = true;
	}
	
	// ================== DRAW ================== //
	if (loadingTimer < loadingTimeMax) {
		drawLoadingLines(loadingTimer * 100 / loadingTimeMax);
	} else if (active || animRunning) {
		drawLoadingLines(100);  // 100%
	} else if (unhandledAnimRunning) {
		drawLoadingLines((unhandledAnimTimeMax - unhandledAnimTimer) * 100 /
		                 unhandledAnimTimeMax);
		unhandledAnimTimer++;
		if (unhandledAnimTimer >= unhandledAnimTimeMax) {
			unhandledAnimRunning = false;
#if GUI_ENABLE_POPUP
			gb.gui.popup("Game not META :(", 30);
#endif  // GUI_ENABLE_POPUP
		}
	}
	
	// ================== Animation ================== //
	if (animRunning) {
		// Sliding rects animation
		for (char i = 0; i < 4; i++) {
			uint16_t animPercentage = 100 * (animTimer - animInterval * i) / animRectTimeMax;
			if (animPercentage >= 100 || animPercentage < 0) continue;
			gb.display.setColor(rectsPattern[i]);
			uint8_t rect_h = animH_start - animPercentage * (animH_start - animH_end) / 100;
			gb.display.fillRect(
			    0, gb.display.height() / 2 + (animPercentage * gb.display.height() / 200),
			    gb.display.width(), rect_h);
			gb.display.fillRect(
			    0, gb.display.height() / 2 - (animPercentage * gb.display.height() / 200) - rect_h,
			    gb.display.width(), rect_h);
		}

		// Text animation //
		// The y pos is always constant
		const uint8_t META_y_pos = 22;
		const uint8_t MODE_y_pos = 36;
		// x pos
		int8_t META_start_pos = -textMeta_w;
		int8_t META_centered_pos = 40 - textMeta_w / 2;
		int8_t META_finish_pos = 80;

		int8_t MODE_start_pos = 80;
		int8_t MODE_centered_pos = 40 - textMode_w / 2;
		int8_t MODE_finish_pos = -textMode_w;
		// Animation
		// What is happening? Well, 10 frames to slide in ("Mode" comes in 5 frames after),
		// 15 fixed (10 for "MODE"), 8 to slide out (both words leave at the same time)
		if (animTimer < 5) {  // "META" slides in
			drawTextMeta(META_start_pos + animTimer * (META_centered_pos - META_start_pos) / 10,
			             META_y_pos);
		} else if (animTimer < 10) {  // "META" and "MODE" slide in
			drawTextMeta(META_start_pos + animTimer * (META_centered_pos - META_start_pos) / 10,
			             META_y_pos);
			drawTextMode(
			    MODE_start_pos - (animTimer - 5) * (MODE_start_pos - MODE_centered_pos) / 10,
			    MODE_y_pos);
		} else if (animTimer < 15) {  // "MODE" slides in "META" fixed
			drawTextMeta(META_centered_pos, META_y_pos);
			drawTextMode(
			    MODE_start_pos - (animTimer - 5) * (MODE_start_pos - MODE_centered_pos) / 10,
			    MODE_y_pos);
		} else if (animTimer < 30) {  // "META" and "MODE" fixed
			drawTextMeta(META_centered_pos, META_y_pos);
			drawTextMode(MODE_centered_pos, MODE_y_pos);
		} else if (animTimer < 38) {  // "META" and "MODE" slide out
			drawTextMeta(
			    META_centered_pos + (animTimer - 30) * (META_finish_pos - META_centered_pos) / 8,
			    META_y_pos);
			drawTextMode(
			    MODE_centered_pos - (animTimer - 30) * (MODE_centered_pos - MODE_finish_pos) / 8,
			    MODE_y_pos);
		} else {  // End of animation. Also activate MetaMode and allow the Gamebuino menu to be opened once again
			animRunning = false;
			active = true;
		}

		animTimer++;
	}
}

void MetaMode::drawLoadingLines(uint8_t percentage) {
	uint8_t colorOffset = gb.frameCount % NUMBER_OF_COLORS_LINE_PATTERN;
	uint8_t lineW = percentage * gb.display.width() / 100;
	uint8_t xOffset = (gb.display.width() - lineW) / 2;  // Offset to center the lines on the screen

	for (char x = 0; x < lineW; x++) {
		gb.display.drawPixel(
		    xOffset + x, 0,
		    linePattern[(x + colorOffset) % NUMBER_OF_COLORS_LINE_PATTERN]);  // Top line
		gb.display.drawPixel(
		    xOffset + x, gb.display.height() - 1,
		    linePattern[(x + colorOffset) % NUMBER_OF_COLORS_LINE_PATTERN]);  // Bottom line
	}
}

void MetaMode::drawTextMeta(int8_t x, int8_t y) {
	Image text(textMeta_w + 1, textMeta_h + 1, ColorMode::index);
	text.setColor(ColorIndex::brown);
	text.drawBitmap(1, 1, textMeta);
	text.setColor(ColorIndex::white);
	text.drawBitmap(0, 0, textMeta);
	gb.display.drawImage(x, y, text);
}

void MetaMode::drawTextMode(int8_t x, int8_t y) {
	Image text(textMode_w, textMode_h, ColorMode::index);
	text.fill(ColorIndex::darkgray);
	text.setColor(ColorIndex::white);
	text.drawBitmap(0, 0, textMode);
	gb.display.drawImage(x, y, text);
}

};  // Namespace Gamebuino_Meta
