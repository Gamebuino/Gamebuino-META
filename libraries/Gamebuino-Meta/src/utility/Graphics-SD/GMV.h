#ifndef _GAMEBUINO_GMV_H_
#define	_GAMEBUINO_GMV_H_

#include "BMP.h"
#include "../Image.h"
#include "../SdFat.h"
#include "../Display-ST7735.h"
#include "../Graphics.h"

namespace Gamebuino_Meta {

class GMV {
public:
	GMV();
	GMV(Image* _img);
	GMV(Image* _img, char* filename);
	bool isValid();
	bool is(Image* _img);
	bool initSave(char* filename);
	uint16_t getFrames();
	void writeHeader();
	void writeFrame();
	void readFrame();
	void setFrame(uint16_t frame);
	void finishSave(char* filename, uint16_t frames, bool output, Display_ST7735* tft);
private:
	File file;
	Image* img;
	void convertFromBMP(BMP& bmp, char* newname);
	void writeColor(File* f, uint16_t color, uint8_t count);
	void writeHeader(File* f);
	void writeFrame(File* f);
	bool valid;
	uint16_t header_size;
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_GMV_H_
