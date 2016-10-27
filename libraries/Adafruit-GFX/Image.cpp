// 
// 
// 

#include "Image.h"


Image::Image(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
	uint16_t bytes = w * h * 2;
	if ((_buffer = (uint16_t *)malloc(bytes))) {
		memset(_buffer, 0, bytes);
	}
}

Image::Image(uint16_t w, uint16_t h, uint16_t* buffer) : Adafruit_GFX(w,h){
	_buffer = buffer;
}


Image::~Image(void) {
	//if ((uint32_t)_buffer < 0x20000000) return; //don't try to deallocate variables from flash
	//if (_buffer) free(_buffer);
}


void Image::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (_buffer) {
		if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;
		_buffer[x + y * WIDTH] = color;
	}
}

void Image::fillScreen(uint16_t color) {
	if (_buffer) {
		uint8_t hi = color >> 8, lo = color & 0xFF;
		if (hi == lo) {
			memset(_buffer, lo, WIDTH * HEIGHT * 2);
		}
		else {
			uint16_t i, pixels = WIDTH * HEIGHT;
			for (i = 0; i<pixels; i++) _buffer[i] = color;
		}
	}
}
