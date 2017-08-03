#ifndef _GAMEBUINO_SD_GFX_H_
#define	_GAMEBUINO_SD_GFX_H_

#include <Arduino.h>
#include "../Image.h"
#include "GMV.h"
#include "../Display-ST7735.h"
#include "../../config/config.h"

namespace Gamebuino_Meta {

class Frame_Handler_SD : public Frame_Handler {
public:
	Frame_Handler_SD(Image* img);
	~Frame_Handler_SD();
	void init(char* filename);
	void next();
	void set(uint16_t frame);
private:
	void lazy_init();
	union {
		GMV* gmv;
		char* lazy_filename;
	};
};

class Recording_Image {
public:
	Recording_Image(GMV& _gmv);
	~Recording_Image();
	void update();
	bool is(Image* img);
	void finish(bool output);
	void setBmpFilename(char* filename);
private:
	char* bmp_filename;
	GMV gmv;
	uint16_t frames;
};

class Graphics_SD {
public:
	static bool startRecording(Image* img, char* filename);
	static void stopRecording(Image* img, bool output);
	static bool save(Image* img, char* filename);
	static void update();
	static void setTft(Display_ST7735* _tft);
};

} // namespace Gamebuino_Meta

#endif /// _GAMEBUINO_SD_GFX_H_
