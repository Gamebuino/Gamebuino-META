#ifndef _GAMEBUINO_SD_GFX_H_
#define	_GAMEBUINO_SD_GFX_H_

#include <Arduino.h>
#include "../Graphics.h"
#include "../Image.h"
#include "../SdFat.h"
#include "../Display-ST7735.h"
#include "../../config/config.h"

namespace Gamebuino_Meta {

#define BUFFPIXEL 16

class BMP {
public:
	BMP(){};
	BMP(Image* img);
	bool isValid();
	void writeHeader(File& file);
	void writeBuffer(File& file);
	void writeFrame(uint32_t frame, uint32_t frames, File& file);
	void setFrames(uint32_t frames);
	Image* img;
	uint32_t imageSize;
private:
	bool valid;
	uint8_t header_size;
	uint8_t depth;
	uint32_t width;
	uint32_t colorTable;
	uint16_t* rambuffer;
	uint32_t imageOffset;
	uint32_t fileSize;
	uint32_t pixel_height;
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
	uint32_t frames;
	void writeColor(uint16_t color, uint8_t count);
	void restoreFrame();
};

class Gamebuino_SD_GFX{
public:
	bool writeImage(Image& img, char *filename);
	bool readImage(Image& img, char *filename);
	bool startRecordImage(Image& img, char *filename);
	void stopRecordImage(Image& img, bool output);
	void stopRecordImage(Image& img, Display_ST7735& tft);
	void update();
private:
	Recording_Image* recording[MAX_IMAGE_RECORDING];
};

} // namespace Gamebuino_Meta

#endif /// _GAMEBUINO_SD_GFX_H_
