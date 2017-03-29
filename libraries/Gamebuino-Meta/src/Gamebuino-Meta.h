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

#include "config.h"


#include <Arduino.h>
#include <SPI.h>
#include "extra/SdFat.h"
extern SdFat SD;


#include "utility/Buttons.h"
#include "utility/Sound.h"
#include "utility/Save.h"

#include "extra/Adafruit_NeoPixel.h"
#include "extra/Gamebuino-Meta-ASFcore.h"
#include "extra/Gamebuino-Meta-ST7735.h"
#include "extra/Gamebuino-Meta-ZeroDMA.h"
#include "extra/Gamebuino-Meta-GFX.h"
#include "extra/Gamebuino-Meta-Image.h"
#include "extra/Gamebuino-Meta-SD-GFX.h"

// make sure that sketches don't screw things up, the SAMD architecture has only one addressable space, thus making the PROGMEM concept unneded
#define F(x) x
#define PROGMEM  

namespace Gamebuino_Meta {
#define TFT_CS		(30u)
#define TFT_RST		(0u)
#define TFT_DC		(31u)
#define SD_CS		(26u)
#define NEOPIX_PIN	(38u)
#define BAT_PIN		(A5)


// implement the bootloader functions as inlines
inline void load_game(const char* filename) {
	((void(*)(const char*))(*((uint32_t*)0x3FF8)))(filename);
}

inline void load_game(char* filename) {
	load_game((const char*)filename);
}

inline void load_loader(void) {
	((void(*)(void))(*((uint32_t*)0x3FF4)))();
}

inline void enter_bootloader(void) {
	((void(*)(void))(*((uint32_t*)0x3FEC)))();
}

class Gamebuino {
public:
	Buttons buttons;
	Sound sound;
	Image display = Image(80,64, ColorMode::RGB565);
	Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
	Adafruit_NeoPixel neoPixels = Adafruit_NeoPixel(8, NEOPIX_PIN, NEO_GRB + NEO_KHZ800);

	void begin();
	void titleScreen(const char* name, const uint8_t *logo);
	void titleScreen(const char* name);
	void titleScreen(const uint8_t* logo);
	void titleScreen();
	bool update();
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
	void popup(const char* text, uint8_t duration);
	//void adjustVolume();
	void changeGame();
	bool settingsAvailable();
	void readSettings();
	void getDefaultName(char* string);
	
	bool collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h);
	bool collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2);
	bool collideBitmapBitmap(int16_t x1, int16_t y1, const uint8_t* b1, int16_t x2, int16_t y2, const uint8_t* b2);

private:
	uint8_t timePerFrame;
	uint32_t nextFrameMillis;
	void updatePopup();
	const char* popupText;
	uint8_t popupTimeLeft;
	bool lowBattery;
	uint16_t battery;
	const char folder_name[sizeof FOLDER_NAME] = FOLDER_NAME;
public:
	Save save = Save(&tft, folder_name);
};

} // namespace Gamebuino_Meta

typedef Gamebuino_Meta::Gamebuino Gamebuino;
typedef Gamebuino_Meta::Image Image;
using Gamebuino_Meta::BlendMode;
using Gamebuino_Meta::ColorMode;
extern Gamebuino gb;

#endif	/* GAMEBUINO_H */
