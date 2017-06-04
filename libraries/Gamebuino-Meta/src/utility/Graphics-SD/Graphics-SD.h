#ifndef _GAMEBUINO_SD_GFX_H_
#define	_GAMEBUINO_SD_GFX_H_

#include <Arduino.h>
#include "../Image.h"
#include "GMV.h"

namespace Gamebuino_Meta {

/*
class RLE_Video {
public:
	RLE_Video(){};
	RLE_Video(Image& _img, File& _file);
	void restoreFrame();
	File file;
	Image* img;
};

class Recording_Image {
public:
	Recording_Image(BMP& _bmp, File& _file, File& _file_tmp);
	void update();
	bool is(Image* img);
	void finish(bool output);
private:
	BMP bmp;
	File file;
	File file_tmp;
	uint32_t frame;
	uint32_t frames;
	void writeColor(uint16_t color, uint8_t count);
};

class Playing_Image {
public:
	Playing_Image(RLE_Video& _rle);
	void update();
	bool is(Image* img);
private:
	RLE_Video rle;
};

class Gamebuino_SD_GFX{
public:
	bool writeImage(Image& img, char *filename);
	bool readImage(Image& img, char *filename);
	bool playImage(Image& img, char *filename);
	bool startRecordImage(Image& img, char *filename);
	void stopRecordImage(Image& img, bool output);
	void stopRecordImage(Image& img, Display_ST7735& tft);
	void update();
private:
	Recording_Image* recording[MAX_IMAGE_RECORDING];
	Playing_Image* playing[MAX_IMAGE_PLAYING];
};
*/

class Frame_Handler_SD : public Frame_Handler {
public:
	Frame_Handler_SD(Image* img);
	~Frame_Handler_SD();
	void init(char* filename);
	void next();
	void set(uint16_t frame);
private:
	GMV* gmv;
};

} // namespace Gamebuino_Meta

#endif /// _GAMEBUINO_SD_GFX_H_
