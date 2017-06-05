#ifndef _GAMEBUINO_BMP_H_
#define	_GAMEBUINO_BMP_H_

#include <Arduino.h>
#include "../SdFat.h"
#include "../Image.h"

namespace Gamebuino_Meta {


class BMP {
public:
	BMP();
	BMP(File* file, Image* img);
	bool isValid();
	uint32_t getCreatorBits();
	void setCreatorBits(File* file, uint32_t bits);
//	void writeHeader(File& file);
//	void writeBuffer(File& file);
//	void writeFrame(uint32_t frame, File& file);
	void readBuffer(File* file, uint16_t* buf, uint16_t height, uint32_t offset);
	void readFrame(uint16_t frame, uint16_t frames, uint16_t height, File* file, uint16_t* buf);
//	void setFrames(uint32_t frames);
	uint32_t getRowSize();
//	uint16_t frames;
	uint8_t depth;
private:
	bool valid;
	uint8_t image_offset;
	uint32_t creatorBits;
	uint16_t width;
	
//	uint8_t depth;
//	uint32_t colorTable;
//	uint16_t* rambuffer;
//	uint32_t imageOffset;
//	uint32_t fileSize;
//	uint32_t pixel_height;
};

} // namespace Gamebuino_Meta

#endif /// _GAMEBUINO_BMP_H_
