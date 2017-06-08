#ifndef _GAMEBUINO_GMV_H_
#define	_GAMEBUINO_GMV_H_

#include "BMP.h"
#include "../Image.h"
#include "../SdFat.h"

namespace Gamebuino_Meta {

class GMV {
public:
	GMV();
	GMV(Image* _img, char* filename);
	bool isValid();
	uint16_t getFrames();
	void writeFrame();
	void readFrame();
	void setFrame(uint16_t frame);
private:
	File file;
	Image* img;
	void convertFromBMP(BMP& bmp, char* newname);
	void writeColor(File* f, uint16_t color, uint8_t count);
	void writeFrame(File* f);
	bool valid;
	uint16_t header_size;
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_GMV_H_
