// 
// 
// 

#include "Image.h"

Image::Image() : Adafruit_GFX(0, 0){
}

Image::Image(uint16_t w, uint16_t h, ColorMode col) : Adafruit_GFX(w, h) {
	colorMode = col;
	allocateBuffer(w, h);
	_width = w;
	_height = h;
}

Image::Image(uint16_t w, uint16_t h, ColorMode col, uint16_t* buffer) : Adafruit_GFX(w, h) {
	colorMode = col;
	_buffer = buffer;
	_width = w;
	_height = h;
}

Image::~Image(void) {
	//if ((uint32_t)_buffer < 0x20000000) return; //don't try to deallocate variables from flash
	//if (_buffer) free(_buffer);
}

void Image::allocateBuffer(uint16_t w, uint16_t h) {
	switch (colorMode) {
	case (ColorMode::INDEX):
	{
		uint16_t bytes = w * h / 2; //4 bits per pixel = 1/2 byte
		if ((_buffer = (uint16_t *)malloc(bytes))) {
			memset(_buffer, 0, bytes);
		}
		break;
	}
	case (ColorMode::RGB565):
	{
		uint16_t bytes = w * h * 2; //16 bits per pixel = 2 bytes
		if ((_buffer = (uint16_t *)malloc(bytes))) {
			memset(_buffer, 0, bytes);
		}
		break;
	}
	}
}


void Image::drawPixel(int16_t x, int16_t y) {
	if (_buffer) {
		if (colorMode == ColorMode::RGB565) {
			if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;
			_buffer[x + y * WIDTH] = color;
			return;
		}
		if (colorMode == ColorMode::INDEX) {
			if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;
			uint16_t addr = x + y * WIDTH; //nibble number
			if (addr & 0x0001) { //odd pixels
				((uint8_t*)_buffer)[addr / 2] &= 0x0F; //clear
				((uint8_t*)_buffer)[addr / 2] |= (color << 4); //set
			}
			else { //even pixels
				((uint8_t*)_buffer)[addr / 2] &= 0xF0; //clear
				((uint8_t*)_buffer)[addr / 2] |= color; //set
			}
			return;
		}
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

void Image::drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w) {
	if ((alpha == 255) && (tint == 0xFFFF)) { //no alpha blending and not tinting
		if (Adafruit_GFX::transparentColor == 0xFFFF) { //no transparent color set
			memcpy(&_buffer[x + y * _width], buffer, w * 2); //fastest copy possible
			return;
		}
		else {
			uint16_t * thisLine = &_buffer[x + y * _width];
			for (uint8_t i = 0; i < w; i++) { //only copy non-transparent-colored pixels
				if (buffer[i] == Adafruit_GFX::transparentColor) continue;
				thisLine[i] = buffer[i];
			}
			return;
		}
	}
	uint16_t * thisLine = &_buffer[x + y * _width];
	int8_t r1[w];
	int8_t b1[w];
	int8_t g1[w];

	//Extract RGB channels from buffer
	for (uint8_t i = 0; i < w; i++) {
		if (buffer[i] == Adafruit_GFX::transparentColor) continue;
		uint16_t color1 = buffer[i];
		r1[i] = color1 & B11111;
		g1[i] = (color1 >> 5) & B111111;
		b1[i] = (color1 >> 11) & B11111;
	}

	//Buffer tinting
	if (tint != 0xFFFF) {
		int8_t tintR = tint & B11111;
		int8_t tintG = (tint >> 5) & B111111;
		int8_t tintB = (tint >> 11) & B11111;
		for (uint8_t i = 0; i < w; i++) {
			if (buffer[i] == Adafruit_GFX::transparentColor) continue;
			r1[i] = r1[i] * tintR / 32;
			g1[i] = g1[i] * tintG / 64;
			b1[i] = b1[i] * tintB / 32;
		}
	}

	//blending
	uint8_t nalpha = 255 - alpha; //complementary of alpha
	switch (blendMode) {
	case BlendMode::BLEND:
	{
		if (alpha < 255) {
			for (uint8_t i = 0; i < w; i++) {
				if (buffer[i] == Adafruit_GFX::transparentColor) continue;
				uint16_t color2 = thisLine[i];
				int16_t r2 = color2 & B11111;
				int16_t g2 = (color2 >> 5) & B111111;
				int16_t b2 = (color2 >> 11) & B11111;
				r1[i] = ((r1[i] * alpha) + (r2 * nalpha)) / 256;
				g1[i] = ((g1[i] * alpha) + (g2 * nalpha)) / 256;
				b1[i] = ((b1[i] * alpha) + (b2 * nalpha)) / 256;
			}
		}
		break;
	}
	case BlendMode::ADD:
	{
		for (uint8_t i = 0; i < w; i++) {
			if (buffer[i] == Adafruit_GFX::transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & B11111;
			int16_t g2 = (color2 >> 5) & B111111;
			int16_t b2 = (color2 >> 11) & B11111;
			r1[i] = min((r1[i] * alpha + r2 * 255) / 256, 31);
			g1[i] = min((g1[i] * alpha + g2 * 255) / 256, 63);
			b1[i] = min((b1[i] * alpha + b2 * 255) / 256, 31);
		}
		break;
	}
	case BlendMode::SUBSTRACT:
	{
		for (uint8_t i = 0; i < w; i++) {
			if (buffer[i] == Adafruit_GFX::transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & B11111;
			int16_t g2 = (color2 >> 5) & B111111;
			int16_t b2 = (color2 >> 11) & B11111;
			r1[i] = max((r2 * 255 - r1[i] * alpha) / 256, 0);
			g1[i] = max((g2 * 255 - g1[i] * alpha) / 256, 0);
			b1[i] = max((b2 * 255 - b1[i] * alpha) / 256, 0);
		}
		break;
	}
	case BlendMode::MULTIPLY:
	{
		for (uint8_t i = 0; i < w; i++) {
			if (buffer[i] == Adafruit_GFX::transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & B11111;
			int16_t g2 = (color2 >> 5) & B111111;
			int16_t b2 = (color2 >> 11) & B11111;
			r1[i] = max((r2 * r1[i]) / 32, 0);
			g1[i] = max((g2 * g1[i]) / 64, 0);
			b1[i] = max((b2 * b1[i]) / 32, 0);
		}
		break;
	}
	case BlendMode::SCREEN:
	{
		for (uint8_t i = 0; i < w; i++) {
			if (buffer[i] == Adafruit_GFX::transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & B11111;
			int16_t g2 = (color2 >> 5) & B111111;
			int16_t b2 = (color2 >> 11) & B11111;
			r1[i] = min(31 - (31 - r2) * (31 - r1[i]) / 32, 31);
			g1[i] = min(63 - (63 - g2) * (63 - g1[i]) / 64, 63);
			b1[i] = min(31 - (31 - b2) * (31 - b1[i]) / 32, 31);
		}
		break;
	}
	}

	for (uint8_t i = 0; i < w; i++) {
		if (buffer[i] == Adafruit_GFX::transparentColor) continue;
		thisLine[i] = (b1[i] << 11) + (g1[i] << 5) + r1[i];
	}
}
