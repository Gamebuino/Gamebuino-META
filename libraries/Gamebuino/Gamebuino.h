/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef GAMEBUINO_H
#define	GAMEBUINO_H

#include <Arduino.h>
#include "utility/Buttons.h"
#include "utility/Sound.h"
#include <Adafruit_ZeroDMA.h>
#include <Adafruit_ASFcore.h>
#include "utility/Adafruit_ST7735.h"
#include "utility/Adafruit_GFX.h"
#include "utility/Image.h"

//GUI settings
#define ENABLE_GUI 0 //enable menu, keyboard, pop-up, volume adjust functions
#define START_MENU_TIMER 255 //40 = 40 frames (2sec) before start menu is skipped, 0 = no start menu, 255 = start menu until you press A
#define KEYBOARD_W 16
#define KEYBOARD_H 8

#define TFT_CS		30
#define TFT_RST		0
#define TFT_DC		31
#define SD_CS		PIN_LED_TXL
#define BTN_CS		PIN_LED_RXL

#define LCDHEIGHT	64
#define LCDWIDTH	80

#define load_game (1);
#define write_flash_page (*((void(*)(const char * page, unsigned char * buffer))(0x7ffa/2)))

#define wrap(i, imax) ((imax+i)%(imax))

class Gamebuino {
public:
    Buttons buttons;
    Sound sound;
    Image display = Image(80,64, ColorMode::RGB565);
	Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

    void begin();
	void titleScreen(const __FlashStringHelper* name, const uint8_t *logo);
	void titleScreen(const __FlashStringHelper* name);
	void titleScreen(const uint8_t* logo);
    void titleScreen();
    boolean update();
	uint8_t startMenuTimer;
    uint32_t frameCount;
    void setFrameRate(uint8_t fps);
	void pickRandomSeed();
	
	uint8_t getCpuLoad();
    uint16_t getFreeRam();
    uint16_t frameDurationMicros;
    uint32_t frameStartMicros, frameEndMicros;
    
    int8_t menu(const char* const* items, uint8_t length);
    void keyboard(char* text, uint8_t length);
    void popup(const __FlashStringHelper* text, uint8_t duration);
	//void adjustVolume();
	void changeGame();
	boolean settingsAvailable();
	void readSettings();
	void getDefaultName(char* string);
	
	boolean collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h);
	boolean collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2);
    boolean collideBitmapBitmap(int16_t x1, int16_t y1, const uint8_t* b1, int16_t x2, int16_t y2, const uint8_t* b2);

private:
    uint8_t timePerFrame;
    uint32_t nextFrameMillis;
    void updatePopup();
    const __FlashStringHelper* popupText;
    uint8_t popupTimeLeft;
};

#endif	/* GAMEBUINO_H */

