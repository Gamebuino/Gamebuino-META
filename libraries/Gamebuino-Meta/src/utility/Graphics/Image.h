// Image.h

#ifndef _GAMEBUINO_META_IMAGE_h
#define _GAMEBUINO_META_IMAGE_h

#include <Arduino.h>
#include "Graphics.h"

namespace Gamebuino_Meta {

class Image : public Graphics {
public:
	Image();
	~Image(void);

	Image(uint16_t w, uint16_t h, ColorMode col);
	Image(uint16_t w, uint16_t h, ColorMode col, uint16_t* buffer);
	void
		deallocateBuffer(),
		allocateBuffer(uint16_t w, uint16_t h),
		freeBuffer(),
		drawPixel(int16_t x, int16_t y),
		fillScreen(Color color),
		drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w);

	uint16_t getBufferSize();

	uint16_t *_buffer;

};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_IMAGE_h
