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
const Color line_pattern[NUMBER_OF_COLORS_LINE_PATTERN] = {  // Color of the bottom and top bars
    WHITE, WHITE, WHITE, WHITE, ORANGE, ORANGE, ORANGE, ORANGE};

const Color rects_pattern[4] = {  // Colors of the sliding rectangles during launch animation
    WHITE, BEIGE, ORANGE, BROWN};

uint8_t current_pattern = 0;

bool anim_running = false;
int16_t anim_timer = 0;
const uint8_t anim_rect_time_max = 10;  // Time per sliding rect

const uint8_t anim_h_start = 18;  // Height of the sliding rects when the start in the center
const uint8_t anim_h_end = 2;     // Their height at the last frame
const uint8_t anim_interval = 7;  // Frames between 2 sliding rectangles

uint8_t loading_timer = 0;
const uint8_t loading_time_max = 40;  // Time in frames you have to hold HOME and MENU

bool anim_unhandled_running = false;
const uint8_t anim_unhandled_time_max = 8;
uint8_t anim_unhandled_timer = 0;

bool MetaMode::isActive() {
	handled = true;
	return active;
};

bool MetaMode::is_using_button_home() {
	return using_button_home;
}

void MetaMode::update() {
	
	// ================== INPUTS ================== //
	if (gb.buttons.repeat(Button::home, 0) && gb.buttons.repeat(Button::menu, 0) && !active &&
	    !anim_running && !anim_unhandled_running && can_activate) {
		loading_timer++;
		if (loading_timer >= loading_time_max / 2)
			using_button_home = true;  // From now on, releasing the home button doesn't open the Gamebuino menu
		
		if (loading_timer == loading_time_max) {  // Once we fully loaded: launch animation, reset
			                                      // timer, and activate metaMode
			if (handled) {
				anim_timer = 0;
				anim_running = true;
			} else {
				anim_unhandled_running = true;
				anim_unhandled_timer = 0;
			}
		}
	} else if (gb.buttons.repeat(Button::home, 0) && gb.buttons.repeat(Button::menu, 0) &&
	           can_deactivate) {
		loading_timer--;           // De-load (like loading, but backwards)
		if (loading_timer <= loading_time_max / 2)
			using_button_home = true;  // From now on, releasing the home button doesn't open the Gamebuino menu
		
		if (loading_timer == 0) {  // Once fully de-loaded, deactivate MetaMode
			active = false;
			can_deactivate = false;
			can_activate = false;  // The user must release then hold the HOME and MENU buttons again to re-activate the mode
		}
	} else if (gb.buttons.released(Button::home) || gb.buttons.released(Button::menu)) {
		can_activate = true;
		if (gb.buttons.released(Button::home))  // Only do this when releasing HOME
			using_button_home = false;  // MetaMode is no longer preventing the Gamebuino menu to open
		if (!active && !anim_running &&
		    !anim_unhandled_running)  // If the user lets go while loading
			loading_timer = 0;
		else if (can_deactivate)  // If the user lets go while de-loading
			loading_timer = loading_time_max;
		else if (active || anim_running) 	// If the user lets go once the MetaMode is armed, it means that they can
											// now press the combination again to deactivate it
			can_deactivate = true;
	}
	
	// ================== DRAW ================== //
	if (loading_timer < loading_time_max) {
		drawLoadingLines(loading_timer * 100 / loading_time_max);
	} else if (active || anim_running) {
		drawLoadingLines(100);  // 100%
	} else if (anim_unhandled_running) {
		drawLoadingLines((anim_unhandled_time_max - anim_unhandled_timer) * 100 /
		                 anim_unhandled_time_max);
		anim_unhandled_timer++;
		if (anim_unhandled_timer >= anim_unhandled_time_max) {
			anim_unhandled_running = false;
#if GUI_ENABLE_POPUP
			gb.gui.popup("Game not META :(", 30);
#endif  // GUI_ENABLE_POPUP
		}
	}
	
	// ================== Animation ================== //
	if (anim_running) {
		// Sliding rects animation
		for (char i = 0; i < 4; i++) {
			uint16_t anim_percentage = 100 * (anim_timer - anim_interval * i) / anim_rect_time_max;
			if (anim_percentage >= 100 || anim_percentage < 0) continue;
			gb.display.setColor(rects_pattern[i]);
			uint8_t rect_h = anim_h_start - anim_percentage * (anim_h_start - anim_h_end) / 100;
			gb.display.fillRect(
			    0, gb.display.height() / 2 + (anim_percentage * gb.display.height() / 200),
			    gb.display.width(), rect_h);
			gb.display.fillRect(
			    0, gb.display.height() / 2 - (anim_percentage * gb.display.height() / 200) - rect_h,
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
		if (anim_timer < 5) {  // "META" slides in
			drawTextMeta(META_start_pos + anim_timer * (META_centered_pos - META_start_pos) / 10,
			             META_y_pos);
		} else if (anim_timer < 10) {  // "META" and "MODE" slide in
			drawTextMeta(META_start_pos + anim_timer * (META_centered_pos - META_start_pos) / 10,
			             META_y_pos);
			drawTextMode(
			    MODE_start_pos - (anim_timer - 5) * (MODE_start_pos - MODE_centered_pos) / 10,
			    MODE_y_pos);
		} else if (anim_timer < 15) {  // "MODE" slides in "META" fixed
			drawTextMeta(META_centered_pos, META_y_pos);
			drawTextMode(
			    MODE_start_pos - (anim_timer - 5) * (MODE_start_pos - MODE_centered_pos) / 10,
			    MODE_y_pos);
		} else if (anim_timer < 30) {  // "META" and "MODE" fixed
			drawTextMeta(META_centered_pos, META_y_pos);
			drawTextMode(MODE_centered_pos, MODE_y_pos);
		} else if (anim_timer < 38) {  // "META" and "MODE" slide out
			drawTextMeta(
			    META_centered_pos + (anim_timer - 30) * (META_finish_pos - META_centered_pos) / 8,
			    META_y_pos);
			drawTextMode(
			    MODE_centered_pos - (anim_timer - 30) * (MODE_centered_pos - MODE_finish_pos) / 8,
			    MODE_y_pos);
		} else {  // End of animation. Also activate MetaMode and allow the Gamebuino menu to be opened once again
			anim_running = false;
			active = true;
		}

		anim_timer++;
	}
}

void MetaMode::drawLoadingLines(uint8_t percentage) {
	uint8_t color_offset = gb.frameCount() % NUMBER_OF_COLORS_LINE_PATTERN;
	uint8_t line_w = percentage * gb.display.width() / 100;
	uint8_t x_offset = (gb.display.width() - line_w) / 2;  // Offset to center the lines on the screen

	for (char x = 0; x < line_w; x++) {
		gb.display.drawPixel(
		    x_offset + x, 0,
		    line_pattern[(x + color_offset) % NUMBER_OF_COLORS_LINE_PATTERN]);  // Top line
		gb.display.drawPixel(
		    x_offset + x, gb.display.height() - 1,
		    line_pattern[(x + color_offset) % NUMBER_OF_COLORS_LINE_PATTERN]);  // Bottom line
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
