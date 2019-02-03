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
 - Julien Giovinazzo
 - Sorunome
*/

#include "MetaMode.h"
#include "../../Gamebuino-Meta.h"

namespace Gamebuino_Meta {

extern Gamebuino* gbptr;

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


void MetaMode::updateButtons() {
	if (gbptr->buttons.repeat(Button::home, 0) && gbptr->buttons.repeat(Button::menu, 0) && !active &&
		!animRunning && !unhandledAnimRunning && canActivate) {
		loadingTimer++;
		
		if (loadingTimer == loadingTimeMax) {  // Once we fully loaded: launch animation, reset timer, and activate metaMode
			if (handled) {
				animTimer = 0;
				animRunning = true;
			} else {
				unhandledAnimRunning = true;
				unhandledAnimTimer = 0;
			}
		} else  {
			usingMenuButton = true;
			usingHomeButton = true;
		}
	} else if (gbptr->buttons.repeat(Button::home, 0) && gbptr->buttons.repeat(Button::menu, 0) &&
			   canDeactivate) {
		loadingTimer--;           // De-load (like loading, but backwards)

		if (loadingTimer == 0) {  // Once fully de-loaded, deactivate MetaMode
			active = false;
			canDeactivate = false;
			canActivate = false;  // The user must release then hold the HOME and MENU buttons again to re-activate the mode
		} else {
			usingMenuButton = true;
			usingHomeButton = true;
		}
	} else {
		if (usingMenuButton && !gbptr->buttons.repeat(Button::menu, 0)) {
			usingMenuButton = false;
		}
		if (usingHomeButton && !gbptr->buttons.repeat(Button::home, 0)) {
			usingHomeButton = false;
		}
	}
	if (!usingMenuButton || !usingHomeButton) {
		canActivate = true;
		if (!active && !animRunning &&
			!unhandledAnimRunning)  // If the user lets go while loading
			loadingTimer = 0;
		else if (canDeactivate)  // If the user lets go while de-loading
			loadingTimer = loadingTimeMax;
		else if (active || animRunning) 	// If the user lets go once the MetaMode is armed, it means that they can
											// now press the combination again to deactivate it
			canDeactivate = true;
	}
	
	// Ban the buttons we are using
	if (usingMenuButton) gbptr->buttons.states[(uint8_t)Button::menu] = 0;
	if (usingHomeButton) gbptr->buttons.states[(uint8_t)Button::home] = 0;
}

void MetaMode::updateAnimations() {
	// ================= Logic ======================= // 
	uint8_t scale = gbptr->display.width() == 80 ? 1 : 2;  // Also handle full res games
	static const uint8_t DISP_W = 80;
	static const uint8_t DISP_H = 64;
	
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
			gbptr->gui.popup("Game not META :(", 30);
#endif  // GUI_ENABLE_POPUP
		}
	}
	
	// ================== Animation ================== //
	if (animRunning) {
		// Sliding rects animation
		for (uint8_t i = 0; i < 4; i++) {
			uint16_t animPercentage = 100 * (animTimer - animInterval * i) / animRectTimeMax;
			if (animPercentage >= 100 || animPercentage < 0) continue;
			gbptr->display.setColor(rectsPattern[i]);
			int16_t rect_h = animH_start - animPercentage * (animH_start - animH_end) / 100;
			gbptr->display.fillRect(
				0, (DISP_H / 2 + (animPercentage * DISP_H / 200)) * scale,
				DISP_W * scale, rect_h * scale);
			gbptr->display.fillRect(
				0, (DISP_H / 2 - (animPercentage * DISP_H / 200) - rect_h) * scale,
				DISP_W * scale, rect_h * scale);
		}

		// Text animation //
		// The y pos is always constant
		static const uint8_t META_y_pos = 22;
		static const uint8_t MODE_y_pos = 36;
		// x pos
		int16_t META_start_pos = -textMeta_w;
		int16_t META_centered_pos = (DISP_W / 2 - textMeta_w / 2);
		int16_t META_finish_pos = DISP_W;
		
		int16_t MODE_start_pos = DISP_W;
		int16_t MODE_centered_pos = (DISP_W / 2 - textMode_w / 2);
		int16_t MODE_finish_pos = -textMode_w;
		// Animation
		// What's happening you say? Well, there are 10 frames to slide in ("MODE" comes in 5 frames after),
		// 15 fixed (10 for "MODE"), 8 to slide out (both words leave at the same time)
		if (animTimer < 5) {  // "META" slides in
			drawTextMeta((META_start_pos + animTimer * (META_centered_pos - META_start_pos) / 10) * scale,
						 META_y_pos * scale);
		} else if (animTimer < 10) {  // "META" and "MODE" slide in
			drawTextMeta((META_start_pos + animTimer * (META_centered_pos - META_start_pos) / 10) * scale,
						 META_y_pos * scale);
			drawTextMode(
				(MODE_start_pos - (animTimer - 5) * (MODE_start_pos - MODE_centered_pos) / 10) * scale,
				MODE_y_pos * scale);
		} else if (animTimer < 15) {  // "MODE" slides in "META" fixed
			drawTextMeta(META_centered_pos * scale, META_y_pos * scale);
			drawTextMode(
				(MODE_start_pos - (animTimer - 5) * (MODE_start_pos - MODE_centered_pos) / 10) * scale,
				MODE_y_pos * scale);
		} else if (animTimer < 30) {  // "META" and "MODE" fixed
			drawTextMeta(META_centered_pos * scale, META_y_pos * scale);
			drawTextMode(MODE_centered_pos * scale, MODE_y_pos * scale);
		} else if (animTimer < 38) {  // "META" and "MODE" slide out
			drawTextMeta(
				(META_centered_pos + (animTimer - 30) * (META_finish_pos - META_centered_pos) / 8) * scale,
				META_y_pos * scale);
			drawTextMode(
				(MODE_centered_pos - (animTimer - 30) * (MODE_centered_pos - MODE_finish_pos) / 8) * scale,
				MODE_y_pos * scale);
		} else {  // End of animation. Also activate MetaMode and allow the Gamebuino menu to be opened once again
			animRunning = false;
			active = true;
		}

		animTimer++;
	}
}

void MetaMode::drawLoadingLines(uint8_t percentage) {
	uint8_t colorOffset = gbptr->frameCount % NUMBER_OF_COLORS_LINE_PATTERN;
	uint8_t lineW = percentage * gbptr->display.width() / 100;
	uint8_t xOffset = (gbptr->display.width() - lineW) / 2;  // Offset to center the lines on the screen

	for (char x = 0; x < lineW; x++) {
		gbptr->display.drawPixel(
			xOffset + x, 0,
			linePattern[(x + colorOffset) % NUMBER_OF_COLORS_LINE_PATTERN]);  // Top line
		gbptr->display.drawPixel(
			xOffset + x, gbptr->display.height() - 1,
			linePattern[(x + colorOffset) % NUMBER_OF_COLORS_LINE_PATTERN]);  // Bottom line
	}
}

void MetaMode::drawTextMeta(int16_t x, int16_t y) {
	uint8_t scale = 1;
	if (gbptr->display.width() != 80) {
		x *= 2;
		y *= 2;
		scale *= 2;
	}
	gbptr->display.setColor(ColorIndex::brown);
	gbptr->display.drawBitmap(x+scale, y+scale, textMeta, scale);
	gbptr->display.setColor(ColorIndex::white);
	gbptr->display.drawBitmap(x, y, textMeta, scale);
}

void MetaMode::drawTextMode(int16_t x, int16_t y) {
	uint8_t scale = 1;
	if (gbptr->display.width() != 80) {
		x *= 2;
		y *= 2;
		scale *= 2;
	}
	gbptr->display.setColor(ColorIndex::darkgray);
	gbptr->display.fillRect(x, y, textMode_w*scale, textMode_h*scale);
	gbptr->display.setColor(ColorIndex::white);
	gbptr->display.drawBitmap(x, y, textMode, scale);
}

};  // Namespace Gamebuino_Meta
