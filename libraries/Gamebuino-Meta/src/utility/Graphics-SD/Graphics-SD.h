#ifndef _GAMEBUINO_SD_GFX_H_
#define	_GAMEBUINO_SD_GFX_H_

#include <Arduino.h>
#include "../Graphics.h"
#include "../Image.h"
#include "../SdFat.h"
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
	void setFrames(uint32_t frames);
	Image* img;
private:
	bool valid;
	uint8_t header_size;
	uint8_t depth;
	uint32_t width;
	uint32_t colorTable;
	uint16_t* rambuffer;
	uint32_t imageOffset;
	uint32_t imageSize;
	uint32_t fileSize;
	uint32_t pixel_height;
};

struct Recording_Image {
	bool recording = false;
	BMP bmp;
	File file;
	uint32_t frames;
};

class Gamebuino_SD_GFX{
public:
	bool writeImage(Image& img, char *filename);
	bool readImage(Image& img, char *filename);
	bool startRecordImage(Image& img, char *filename);
	void stopRecordImage(Image& img);
	void update();
private:
	Recording_Image recording[MAX_IMAGE_RECORDING];
};

} // namespace Gamebuino_Meta

#endif /// _GAMEBUINO_SD_GFX_H_
