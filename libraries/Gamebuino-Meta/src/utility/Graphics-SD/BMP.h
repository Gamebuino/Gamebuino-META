#ifndef _GAMEBUINO_BMP_H_
#define	_GAMEBUINO_BMP_H_

#include <Arduino.h>
#include "../SdFat.h"
#include "../Image.h"

namespace Gamebuino_Meta {


class BMP {
public:
	BMP();
	BMP(Image* img, uint16_t frames);
	BMP(File* file, Image* img);
	bool isValid();
	uint32_t getCreatorBits();
	void setCreatorBits(uint32_t bits, File* file);
	void readBuffer(uint16_t* buf, uint32_t offset, File* file);
	void readFrame(uint16_t frame, uint16_t* buf, File* file);
	uint32_t getRowSize();
	
	uint32_t writeHeader(File* file);
	void writeBuffer(uint16_t* buffer, File* file);
	void writeFrame(uint16_t frame, uint16_t* buffer, File* file);
	uint8_t depth;
private:
	bool valid;
	uint8_t image_offset;
	uint32_t creatorBits;
	uint16_t width;
	uint16_t height;
	uint16_t frames;
	uint8_t indexMap[16];
};

} // namespace Gamebuino_Meta

#endif /// _GAMEBUINO_BMP_H_
