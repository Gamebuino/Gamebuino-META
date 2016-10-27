// Image.h

#ifndef _IMAGE_h
#define _IMAGE_h

#include "Arduino.h"
#include "Adafruit_GFX.h"

class Image : public Adafruit_GFX {
public:
	Image(uint16_t w, uint16_t h);
	Image(uint16_t w, uint16_t h, uint16_t* buffer);
	~Image(void);
	void
		drawPixel(int16_t x, int16_t y, uint16_t color),
		fillScreen(uint16_t color);

	uint16_t * _buffer;

	__attribute__((always_inline))
		void drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w) {
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
				r1[i] = min(31 - (31 - r2) * (31 - r1[i]) / 32 , 31);
				g1[i] = min(63 - (63 - g2) * (63 - g1[i]) / 64 , 63);
				b1[i] = min(31 - (31 - b2) * (31 - b1[i]) / 32 , 31);
			}
			break;
		}
	}

		for (uint8_t i = 0; i < w; i++) {
			if (buffer[i] == Adafruit_GFX::transparentColor) continue;
			thisLine[i] = (b1[i] << 11) + (g1[i] << 5) + r1[i];
		}
	}

};

#endif

