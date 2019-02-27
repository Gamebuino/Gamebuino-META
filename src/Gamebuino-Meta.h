/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2014, 2016-2017

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
 - Aurelien Rodot
 - Sorunome
*/

#ifndef _GAMEBUINO_META_H_
#define	_GAMEBUINO_META_H_

#ifndef __SKETCH_NAME__
#error "Please compile using the Gamebuino Meta board!"
#endif

#include "config/config.h"

#if USE_SDFAT
#include <SPI.h>
#include "utility/SdFat.h"
extern SdFat SD;
#endif

#include "utility/Buttons.h"
#include "utility/Sound.h"
#include "utility/Save.h"
#include "utility/Bootloader.h"
#include "utility/Gui.h"
#include "utility/MetaMode.h"
#include "utility/Collide.h"

#include "utility/Adafruit_NeoPixel.h"
#include "utility/Display-ST7735.h"
#include "utility/Graphics.h"
#include "utility/Image.h"
#include "utility/Language.h"

// make sure that sketches don't screw things up, the SAMD architecture has only one addressable space, thus making the PROGMEM concept unneded
#ifndef F
#define F(x) x
#endif

#ifndef PROGMEM
#define PROGMEM  
#endif

#ifdef GAMEBUINO_COMPAT_MODE
// we need the display definition from the compat lib....
// we need to do it here as the arduino IDE doesn't add all include paths from the start
// and thus Gamebuino_Compat::Display will not find Gamebuino_Meta::Image yet
#include <utility/Display_Compat.h>
#include <utility/Sound_Compat.h>
#endif

namespace Gamebuino_Meta {
#define TFT_CS		(30u)
#define TFT_DC		(31u)
#define SD_CS		(26u)
#define NEOPIX_PIN	(38u)
#define BAT_PIN		(A5)

#define wrap(i, imax) ((imax+i)%(imax))

class Gamebuino {
public:
	Buttons buttons;
#ifdef GAMEBUINO_COMPAT_MODE
	Gamebuino_Compat::Display display;
	Gamebuino_Compat::Sound sound;
#else
	Image display = DISPLAY_CONSTRUCTOR;
	Sound sound;
#endif
	Display_ST7735 tft = Display_ST7735(TFT_CS, TFT_DC);
	Image lights = Image(2, 4, ColorMode::rgb565);
	Save save;
	Save settings;
	Language language;
	Bootloader bootloader;
	Gui gui;
	MetaMode metaMode;
	Collide collide;

	void begin();
	void startScreen();
	void titleScreen();
	bool update();
	void waitForUpdate();
	void updateDisplay();
	uint8_t startMenuTimer;
	uint32_t frameCount;
	void setFrameRate(uint8_t fps);
	void pickRandomSeed();
	
	uint8_t getCpuLoad();
	uint16_t getFreeRam();
	uint32_t frameDurationMicros;
	uint32_t frameStartMicros;
	bool frameEndFlag;
	
	void checkHomeMenu();
	void homeMenu();
	
	void changeGame();
	void getDefaultName(char* string);
	
	bool collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h);  // deprecated. Use collide.pointRect() instead
	bool collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2);  // deprecated. Use collide.rectRect() instead
	bool collideBitmapBitmap(int16_t x1, int16_t y1, const uint8_t* b1, int16_t x2, int16_t y2, const uint8_t* b2);
	
	Color createColor(uint8_t r, uint8_t g, uint8_t b);
	uint8_t getTimePerFrame();
	
	void setScreenRotation(Rotation r);
	Rotation getScreenRotation();
	
	// so that we know when the object is actually initialized
	// importent for proper detection of what gb.display.width() and gb.display.height() return outside any function
	// unfortunatelly the arduino platform.txt doesn't allow us to change the order of object files, which would be the prefered way
	// as the Gamebuino constructor would be called then first, for sure
	
	bool inited = false;
	bool sdInited = false;
private:
	Adafruit_NeoPixel neoPixels = Adafruit_NeoPixel(8, NEOPIX_PIN, NEO_GRB + NEO_KHZ800);
	uint8_t timePerFrame;
	uint32_t nextFrameMillis;
	void drawLogo(Graphics& g, int8_t x, int8_t y, uint8_t scale);
	int16_t framesDisplayRecording = -1;
#ifdef FOLDER_NAME
	const char folder_name[sizeof FOLDER_NAME] = FOLDER_NAME;
#else
	char folder_name[sizeof __SKETCH_NAME__ - 4];
#endif
};

extern const uint16_t buttonsIconsData[];
extern const uint16_t arrowsIconsData[];

} // namespace Gamebuino_Meta

#ifdef GAMEBUINO_COMPAT_MODE
#include <Gamebuino-Compat.h>
#else
#if AUTOCREATE_OBJECT
using Gamebuino_Meta::Gamebuino;
extern Gamebuino gb;
#endif
#endif

#endif	// _GAMEBUINO_META_H_
