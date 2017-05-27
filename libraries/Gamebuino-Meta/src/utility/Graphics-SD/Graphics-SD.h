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
	BMP(File& file, Image* img);
	bool isValid();
	void writeHeader(File& file);
	void writeBuffer(File& file);
	void readBuffer(File& file);
	void readBuffer(File& file, uint32_t offset);
	void writeFrame(uint32_t frame, File& file);
	void readFrame(uint32_t frame, File& file);
	void setFrames(uint32_t frames);
	uint32_t getRowSize();
	Image* img;
	uint32_t imageSize;
	uint32_t frames = 1;
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

} // namespace Gamebuino_Meta

#endif /// _GAMEBUINO_SD_GFX_H_
