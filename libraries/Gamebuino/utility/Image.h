// Image.h

#ifndef _IMAGE_h
#define _IMAGE_h

#include "Arduino.h"
#include "Adafruit_GFX.h"

class Image : public Adafruit_GFX {
public:
	Image();
	~Image(void);

	Image(uint16_t w, uint16_t h, ColorMode col);
	Image(uint16_t w, uint16_t h, ColorMode col, uint16_t* buffer);
	void
		allocateBuffer(uint16_t w, uint16_t h),
		freeBuffer(),
		drawPixel(int16_t x, int16_t y, uint16_t color),
		fillScreen(uint16_t color),
		drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w);

	uint16_t *_buffer;

};

#endif

