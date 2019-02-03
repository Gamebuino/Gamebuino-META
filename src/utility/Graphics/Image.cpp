/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2016-2017

This is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL)
as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License (LGPL) for more details.

You should have received a copy of the GNU Lesser General Public
License (LGPL) along with the library.
If not, see <http://www.gnu.org/licenses/>.

Authors:
 - Aurelien Rodot
 - Sorunome
*/

#include "Image.h"
#include "../Graphics-SD.h"
#include "../../Gamebuino-Meta.h"

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef min
#define min(x, y) ((x < y) ? x : y)
#endif

namespace Gamebuino_Meta {

extern Gamebuino* gbptr;

Frame_Handler::Frame_Handler(Image* _img) {
	img = _img;
	buf = img->_buffer;
	bufferSize = img->bufferSize;
}

Frame_Handler::~Frame_Handler() {
	if (buf) {
		img->_buffer = buf;
		img->bufferSize = bufferSize;
	}
}

void Frame_Handler::first() {
	set(0);
}

uint32_t Frame_Handler::getBufferSizeWithFrames() {
	return img->getBufferSize();
}

void Frame_Handler::allocateBuffer() {
	uint32_t bytes = getBufferSizeWithFrames();
	if (buf && (bytes <= bufferSize)) {
		img->_buffer = buf;
		return;
	}
	if (buf && (uint32_t)buf >= 0x20000000) {
		gb_free(buf);
	}
	if ((buf = (uint16_t *)gb_malloc(bytes))) {
		memset(buf, 0, bytes);
		bufferSize = bytes;
	} else {
		// we weren't able to allocate anything :(
		img->_width = img->_height = 0;
		bufferSize = 0;
	}
	img->_buffer = buf;
}

Frame_Handler_Mem::Frame_Handler_Mem(Image* _img) : Frame_Handler(_img) {
	
}

Frame_Handler_Mem::~Frame_Handler_Mem() {
	if (buf) {
		img->_buffer = buf;
		img->bufferSize = 0;
	}
}

void Frame_Handler_Mem::set(uint16_t frame) {
	uint32_t buf_size = img->getBufferSize();
	img->_buffer = (uint16_t*)((uint32_t)buf + (buf_size * frame));
}

void Frame_Handler_Mem::next() {
	img->_buffer = (uint16_t*)((uint32_t)img->_buffer + img->getBufferSize());
}

Frame_Handler_RAM::Frame_Handler_RAM(Image* _img) : Frame_Handler_Mem(_img) {
	allocateBuffer();
}

Frame_Handler_RAM::~Frame_Handler_RAM() {
	if (buf) {
		img->_buffer = buf;
		img->bufferSize = bufferSize;
	}
}

uint32_t Frame_Handler_RAM::getBufferSizeWithFrames() {
	return img->getBufferSize() * img->frames;
}


/********
 * start of actual image class
 ********/

void Image::freshStart() {
	_buffer = 0;
	frame_handler = 0;
	bufferSize = 0;
	isObjectCopy = false;
}

Image::Image() : Graphics(0, 0) {
	freshStart();
}

Image::Image(const Image& img) : Graphics(0, 0) { // copy constructor!
	// copy over the thing
	if (!frame_handler) {
		memcpy(this, &img, sizeof(Image));
		isObjectCopy = false;
	} else {
		memcpy(this, &img, sizeof(Image));
		isObjectCopy = true;
	}
}


// ram constructors
Image::Image(uint16_t w, uint16_t h, uint16_t frames, uint8_t fl) : Graphics(w, h) {
	freshStart();
	init(w, h, frames, fl);
}
void Image::init(uint16_t w, uint16_t h, uint16_t frames, uint8_t fl) {
	init(w, h, ColorMode::rgb565, frames, fl);
}

Image::Image(uint16_t w, uint16_t h, ColorMode col, uint16_t frames, uint8_t fl) : Graphics(w, h) {
	freshStart();
	init(w, h, col, frames, fl);
}
void Image::init(uint16_t w, uint16_t h, ColorMode col, uint16_t _frames, uint8_t fl) {
	if (isObjectCopy) {
		return;
	}
	if (frame_handler) {
		delete frame_handler;
	} else {
		bufferSize = 0;
		_buffer = 0;
	}
	transparentColor = 0xF81F;
	frames = _frames;
	colorMode = col;
	_width = w;
	_height = h;
	frame_looping = fl;
	frame_handler = new Frame_Handler_RAM(this);
	frame_loopcounter = 0;
	last_frame = (gbptr->frameCount & 0xFF) - 1;
	setFrame(0);
}

// flash constructors
Image::Image(const uint16_t* buffer) : Graphics(0, 0) {
	freshStart();
	init(buffer);
}
void Image::init(const uint16_t* buffer) {
	if (isObjectCopy) {
		return;
	}
	if (frame_handler) {
		delete frame_handler;
	}
	bufferSize = 0;
	if (_buffer && (uint32_t)_buffer >= 0x20000000) {
		gb_free(_buffer);
	}
	uint16_t* buf = (uint16_t*)buffer;
	_width = *(buf++);
	_height = *(buf++);
	frames = *(buf++);
	frame_looping = *(buf++);
	transparentColor = *(buf++);
	colorMode = (ColorMode)(*(buf++));
	if (colorMode == ColorMode::index) {
		if (transparentColor > 0x0F) {
			useTransparentIndex = false;
		} else {
			uint8_t c = transparentColor;
			transparentColorIndex = c;
			useTransparentIndex = true;
		}
	}
	
	_buffer = buf;
	frame_handler = new Frame_Handler_Mem(this);
	frame_loopcounter = 0;
	last_frame = (gbptr->frameCount & 0xFF) - 1;
	setFrame(0);
}

// flash indexed constructors
Image::Image(const uint8_t* buffer) : Graphics(0, 0) {
	freshStart();
	init(buffer);
}
void Image::init(const uint8_t* buffer) {
	if (isObjectCopy) {
		return;
	}
	if (frame_handler) {
		delete frame_handler;
	}
	if (bufferSize && _buffer && (uint32_t)_buffer >= 0x20000000) {
		gb_free(_buffer);
	}
	
	uint8_t* buf = (uint8_t*)buffer;
	_width = *(buf++);
	_height = *(buf++);
	frames = *(buf++);
	frames += (*(buf++)) << 8;
	frame_looping = *(buf++);
	transparentColor = *(buf++);
	colorMode = (ColorMode)(*(buf++));
	if (colorMode == ColorMode::index) {
		if (transparentColor > 0x0F) {
			useTransparentIndex = false;
		} else {
			uint8_t c = transparentColor;
			transparentColorIndex = c;
			useTransparentIndex = true;
		}
	}
	
	_buffer = (uint16_t*)buf;
	frame_handler = new Frame_Handler_Mem(this);
	frame_loopcounter = 0;
	last_frame = (gbptr->frameCount & 0xFF) - 1;
	setFrame(0);
}

// SD constructors
Image::Image(char* filename, uint8_t fl) : Graphics(0, 0) {
	freshStart();
	init(filename, fl);
}
void Image::init(char* filename, uint8_t fl) {
	init(0, 0, filename, fl);
}

Image::Image(uint16_t w, uint16_t h, char* filename, uint8_t fl) : Graphics(w, h) {
	freshStart();
	init(w, h, filename, fl);
}
void Image::init(uint16_t w, uint16_t h, char* filename, uint8_t fl) {
	if (isObjectCopy) {
		return;
	}
	if (frame_handler) {
		delete frame_handler;
	} else {
		bufferSize = 0;
		_buffer = 0;
	}
	transparentColor = 0;
	_width = w;
	_height = h;
	frame_looping = fl;
	frame = 0;
	frame_handler = new Frame_Handler_SD(this);
	// for the SD handler we do NOT set frame to zero
	// unlike the other handlers the SD handler must be lazy-inited
	// and calling setFrame(0) here would trigger the lazy init already
	//setFrame(0);
	((Frame_Handler_SD*)frame_handler)->init(filename);
	frame_loopcounter = 0;
	last_frame = (gbptr->frameCount & 0xFF) - 1;
}


Image::~Image() {
	if (isObjectCopy) {
		return;
	}
	if (_buffer && (uint32_t)_buffer >= 0x20000000) {
		gb_free(_buffer);
		_buffer = 0;
	}
	delete frame_handler;
}

void Image::drawFastHLine(int16_t x, int16_t y, int16_t w) {
	// Don't draw if we are outside the screen
	if (x + w <= 0 || x >= _width || y < 0 || y >= _height) return;
	
	if (colorMode == ColorMode::rgb565) {
		if (w < 0) {
			x += w + 1;
			w = abs(w);
		}
		
		// Clamp value so we don't go outside the buffer
		uint16_t new_x = max(x,0);
		uint32_t bound = new_x + min(w - (new_x - x),_width-new_x);

		if (new_x%2 == 1){
			_buffer[y * _width + new_x] = (uint16_t)color.c;
			new_x ++;
		}
		
		_buffer[y * _width + bound-1] = (uint16_t)color.c;
		for (uint32_t i = new_x>>1; i < (bound>>1); i++){
			((uint32_t*)_buffer)[(y * (_width>>1)) + i] = ((uint16_t)color.c << 16) | (uint16_t)color.c;
		}
		return;
	}
	drawLine(x, y, x+w-1, y);
}

uint16_t Image::getBufferSize() {
	uint16_t bytes = 0;
	if (colorMode == ColorMode::index) {
		// 4 bits per pixel = 1/2 byte
		// add 1 to width to ceil the number, rather than flooring
		bytes = ((_width + 1) / 2) * _height;
	} else if (colorMode == ColorMode::rgb565) {
		bytes = _width * _height * 2; //16 bits per pixel = 2 bytes
	}
	return bytes;
}

bool Image::startRecording(char* filename) {
	return !isObjectCopy && Graphics_SD::startRecording(this, filename);
}

void Image::stopRecording(bool output) {
	if (!isObjectCopy) {
		Graphics_SD::stopRecording(this, output);
	}
}

bool Image::save(char* filename) {
	return !isObjectCopy && Graphics_SD::save(this, filename);
}

void Image::allocateBuffer() {
	if (!isObjectCopy) {
		frame_handler->allocateBuffer();
	}
}

uint16_t Image::getPixel(int16_t x, int16_t y) {
	if (colorMode == ColorMode::rgb565) {
		return _buffer[x + y * _width];
	} else {
		uint16_t addr = ((_width + 1) / 2) * y + x / 2;
		uint8_t* buf = (uint8_t*)_buffer;
		if (!(x % 2)) { //odd pixels
			return buf[addr] >> 4;
		} else {
			return buf[addr] & 0x0F;
		}
	}
}

Color Image::getPixelColor(int16_t x, int16_t y) {
	uint16_t c = getPixel(x, y);
	if (colorMode == ColorMode::rgb565) {
		return (Color)c;
	} else {
		return colorIndex[c];
	}
}

ColorIndex Image::getPixelIndex(int16_t x, int16_t y) {
	uint16_t c = getPixel(x, y);
	if (colorMode == ColorMode::rgb565) {
		return rgb565ToIndex((Color)c);
	} else {
		return (ColorIndex)c;
	}
}

void Image::nextFrame() {
	if (frames) {
		if (frames == 1 || !frame_looping || last_frame == (gbptr->frameCount & 0xFF)) {
			return;
		}
	}
	last_frame = gbptr->frameCount & 0xFF;
	if (frames)  {
		frame_loopcounter++;
		if (frame_loopcounter < frame_looping) {
			return;
		}
	}
	frame_loopcounter = 0;
	if ((frame + 1) >= frames) {
		frame = 0;
		frame_handler->first();
	} else {
		frame_handler->next();
		frame++;
	}
}

void Image::setFrame(uint16_t _frame) {
	if (frames == 1) {
		return;
	}
	if (_frame >= frames) {
		_frame = frames - 1;
	}
	frame = _frame;
	frame_handler->set(frame);
	last_frame = gbptr->frameCount & 0xFF; // we already loaded this frame!
	frame_loopcounter = 0;
}

void Image::_drawPixel(int16_t x, int16_t y) {
	if (!_buffer) {
		return;
	}
	if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
		return;
	}
	if (colorMode == ColorMode::rgb565) {
		_buffer[x + y * _width] = (uint16_t)color.c;
		return;
	}
	if (colorMode == ColorMode::index) {
		uint16_t addr = ((_width + 1) / 2) * y + x / 2;
		uint8_t* buf = (uint8_t*)_buffer;
		if (!(x % 2)) { //odd pixels
			buf[addr] &= 0x0F; //clear
			buf[addr] |= (uint8_t)color.iu; //set
		} else { //even pixels
			buf[addr] &= 0xF0; //clear
			buf[addr] |= (uint8_t)color.i; //set
		}
		return;
	}
}

void Image::_fill() {
	if (_buffer) {
		if (colorMode == ColorMode::rgb565) {
			uint8_t hi = (uint16_t)color.c >> 8;
			uint8_t lo = (uint16_t)color.c & 0xFF;
			if (hi == lo) {
				memset(_buffer, lo, _width * _height * 2);
			} else {
				uint16_t i, pixels = _width * _height;
				for (i = 0; i<pixels; i++) _buffer[i] = (uint16_t)color.c;
			}
		}
		
		if (colorMode == ColorMode::index) {
			uint8_t pack = ((uint8_t)color.i) | ((uint8_t)color.iu);
			memset(_buffer, pack, _width * _height / 2);
		}
	}
}

void Image::drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, Image& img) {
	if (colorMode == ColorMode::index) {
		// TODO: transparent index color
		uint8_t *src = (uint8_t*)buffer;
		uint8_t *dst = (uint8_t*)_buffer + y*((_width + 1) / 2) + x/2;
		if (!img.useTransparentIndex) {
			if (x % 2) {
				*dst = (*dst & 0xF0) | (*(src++) & 0x0F);
				dst++;
				w--;
			}
			memcpy(dst, src, w / 2);
			if (w % 2) {
				dst += (w/2);
				*dst = (*dst & 0x0F) | (src[w/2] & 0xF0);
			}
		} else {
			if (x % 2) {
				uint8_t p = *(src++) & 0x0F;
				if (p != img.transparentColorIndex) {
					*dst = (*dst & 0xF0) | p;
				}
				dst++;
				w--;
			}
			for (uint16_t i = 0; i < (w / 2); i++) {
				uint8_t px = *(src++);
				uint8_t hi = px >> 4;
				uint8_t lo = px & 0x0F;
				if (hi == img.transparentColorIndex && lo == img.transparentColorIndex) {
					// both are transparent, nothing to do
				} else if (hi == img.transparentColorIndex) {
					*dst = (*dst & 0xF0) | lo;
				} else if (lo == img.transparentColorIndex) {
					*dst = (*dst & 0x0F) | (hi << 4);
				} else {
					*dst = px;
				}
				dst++;
			}
			if (w % 2) {
				uint8_t hi = *src >> 4;
				if (hi != img.transparentColorIndex) {
					*dst = (*dst & 0x0F) | (*src & 0xF0);
				}
			}
		}
		return;
	}
	if ((alpha == 255) && (tint == 0xFFFF)) { //no alpha blending and not tinting
		if ((!img.transparentColor && img.colorMode == ColorMode::rgb565) || (img.transparentColor == 1 && img.colorMode == ColorMode::index)) { //no transparent color set
			memcpy(&_buffer[x + y * _width], buffer, w * 2); //fastest copy possible
			return;
		} else {
			uint16_t * thisLine = &_buffer[x + y * _width];
			for (uint8_t i = 0; i < w; i++) { //only copy non-transparent-colored pixels
				if (buffer[i] == img.transparentColor) continue;
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
		if (img.transparentColor && buffer[i] == img.transparentColor) continue;
		uint16_t color1 = buffer[i];
		r1[i] = color1 & 0b11111;
		g1[i] = (color1 >> 5) & 0b111111;
		b1[i] = (color1 >> 11) & 0b11111;
	}

	//Buffer tinting
	if (tint != 0xFFFF) {
		int8_t tintR = tint & 0b11111;
		int8_t tintG = (tint >> 5) & 0b111111;
		int8_t tintB = (tint >> 11) & 0b11111;
		for (uint8_t i = 0; i < w; i++) {
			if (img.transparentColor && buffer[i] == img.transparentColor) continue;
			r1[i] = r1[i] * tintR / 32;
			g1[i] = g1[i] * tintG / 64;
			b1[i] = b1[i] * tintB / 32;
		}
	}

	//blending
	uint8_t nalpha = 255 - alpha; //complementary of alpha
	switch (blendMode) {
	case BlendMode::blend:
		if (alpha < 255) {
			for (uint8_t i = 0; i < w; i++) {
				if (img.transparentColor && buffer[i] == img.transparentColor) continue;
				uint16_t color2 = thisLine[i];
				int16_t r2 = color2 & 0b11111;
				int16_t g2 = (color2 >> 5) & 0b111111;
				int16_t b2 = (color2 >> 11) & 0b11111;
				r1[i] = ((r1[i] * alpha) + (r2 * nalpha)) / 256;
				g1[i] = ((g1[i] * alpha) + (g2 * nalpha)) / 256;
				b1[i] = ((b1[i] * alpha) + (b2 * nalpha)) / 256;
			}
		}
		break;
	case BlendMode::add:
		for (uint8_t i = 0; i < w; i++) {
			if (img.transparentColor && buffer[i] == img.transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & 0b11111;
			int16_t g2 = (color2 >> 5) & 0b111111;
			int16_t b2 = (color2 >> 11) & 0b11111;
			r1[i] = min((r1[i] * alpha + r2 * 255) / 256, 31);
			g1[i] = min((g1[i] * alpha + g2 * 255) / 256, 63);
			b1[i] = min((b1[i] * alpha + b2 * 255) / 256, 31);
		}
		break;
	case BlendMode::subtract:
		for (uint8_t i = 0; i < w; i++) {
			if (img.transparentColor && buffer[i] == img.transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & 0b11111;
			int16_t g2 = (color2 >> 5) & 0b111111;
			int16_t b2 = (color2 >> 11) & 0b11111;
			r1[i] = max((r2 * 255 - r1[i] * alpha) / 256, 0);
			g1[i] = max((g2 * 255 - g1[i] * alpha) / 256, 0);
			b1[i] = max((b2 * 255 - b1[i] * alpha) / 256, 0);
		}
		break;
	case BlendMode::multiply:
		for (uint8_t i = 0; i < w; i++) {
			if (img.transparentColor && buffer[i] == img.transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & 0b11111;
			int16_t g2 = (color2 >> 5) & 0b111111;
			int16_t b2 = (color2 >> 11) & 0b11111;
			r1[i] = max((r2 * r1[i]) / 32, 0);
			g1[i] = max((g2 * g1[i]) / 64, 0);
			b1[i] = max((b2 * b1[i]) / 32, 0);
		}
		break;
	case BlendMode::screen:
		for (uint8_t i = 0; i < w; i++) {
			if (img.transparentColor && buffer[i] == img.transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & 0b11111;
			int16_t g2 = (color2 >> 5) & 0b111111;
			int16_t b2 = (color2 >> 11) & 0b11111;
			r1[i] = min(31 - (31 - r2) * (31 - r1[i]) / 32, 31);
			g1[i] = min(63 - (63 - g2) * (63 - g1[i]) / 64, 63);
			b1[i] = min(31 - (31 - b2) * (31 - b1[i]) / 32, 31);
		}
		break;
	}

	for (uint8_t i = 0; i < w; i++) {
		if (img.transparentColor && buffer[i] == img.transparentColor) continue;
		thisLine[i] = (b1[i] << 11) + (g1[i] << 5) + r1[i];
	}
}

void Image::drawChar(int16_t x, int16_t y, unsigned char c, uint8_t size) {
	if(gfxFont) {
		Graphics::drawChar(x, y, c, size);
		return;
	}
	if((x >= _width)            || // Clip right
		 (y >= _height)           || // Clip bottom
		 ((x + fontWidth * size - 1) < 0) || // Clip left
		 ((y + fontHeight * size - 1) < 0))   // Clip top
		return;
	
	if (size == 2 && colorMode == ColorMode::index) {
		if (x < 0 || y < 0 || x >= _width - fontWidth*size || y >= _height - fontHeight*size) {
			Graphics::drawChar(x, y, c, size);
			return;
		}
		if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior
		if (c >= 0x80) c -= 0x20;
		if (!(x % 2)) {
			uint8_t fg = color.iu | color.i;
			uint8_t bg = bgcolor.iu | bgcolor.i;
			uint8_t* buf = (uint8_t*)_buffer;
			uint8_t img_bytewidth = (_width + 1) / 2;
			buf += y*img_bytewidth + (x / 2);
			uint8_t* _buf = buf;
			uint8_t font_bytewidth = fontWidth - 1;
			for (uint8_t i = 0; i < fontWidth; i++) {
				uint8_t line;
				if (i == font_bytewidth) {
					line = 0;
				} else {
					line = font[c*font_bytewidth + i];
				}
				for (uint8_t j = 0; j < fontHeight; j++) {
					if (line & 0x01) {
						*buf = fg;
						buf += img_bytewidth;
						*buf = fg;
						buf += img_bytewidth;
					} else if (fg != bg) {
						*buf = bg;
						buf += img_bytewidth;
						*buf = bg;
						buf += img_bytewidth;
					} else {
						buf += img_bytewidth*2;
					}
					line >>= 1;
				}
				_buf++;
				buf = _buf;
			}
			
			return;
		} else {
			uint8_t fg1 = color.i;
			uint8_t fg2 = color.iu;
			uint8_t bg1 = bgcolor.i;
			uint8_t bg2 = bgcolor.iu;
			
			uint8_t* buf = (uint8_t*)_buffer;
			uint8_t img_bytewidth = (_width + 1) / 2;
			buf += y*img_bytewidth + (x / 2);
			img_bytewidth--;
			uint8_t* _buf = buf;
			uint8_t font_bytewidth = fontWidth - 1;
			for (uint8_t i = 0; i < fontWidth; i++) {
				uint8_t line;
				if (i == font_bytewidth) {
					line = 0;
				} else {
					line = font[c*font_bytewidth + i];
				}
				for (uint8_t j = 0; j < fontHeight; j++) {
					uint8_t b1 = *buf;
					uint8_t b2 = *(buf + 1);
					b1 &= 0xF0;
					b2 &= 0x0F;
					if (line & 0x01) {
						b1 |= fg1;
						b2 |= fg2;
						*(buf++) = b1;
						*buf = b2;
						buf += img_bytewidth;
						*(buf++) = b1;
						*buf = b2;
						buf += img_bytewidth;
					} else if (fg1 != bg1) {
						b1 |= bg1;
						b2 |= bg2;
						*(buf++) = b1;
						*buf = b2;
						buf += img_bytewidth;
						*(buf++) = b1;
						*buf = b2;
						buf += img_bytewidth;
					} else {
						buf += (img_bytewidth+1)*2;
					}
					line >>= 1;
				}
				_buf++;
				buf = _buf;
			}
			return;
		}
	}
	
	
	Graphics::drawChar(x, y, c, size);
}

void Image::drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap) {
	uint8_t w = *(bitmap++);
	uint8_t h = *(bitmap++);
	
	if ((x >= _width) || (y >= _height) || (x + w <= 0) || (x + h <= 0)) {
		return;
	}
	uint8_t bw = (w + 7) / 8;
	uint8_t _x = x;
	if (colorMode == ColorMode::index) {
		uint8_t bw = (w + 7) / 8;
		if (y < 0) {
			h += y;
			bitmap -= bw*y;
			y = 0;
		}
		if (y + h > _height) {
			h = _height - y;
		}
		uint8_t x1 = max(0, x);
		uint8_t x2 = min(_width, x + w);
		
		bitmap += (x1 - x) / 8;
		uint8_t first_bitmap_mask = 0x80 >> ((x1 - x) & 7);
		uint16_t bufBytewidth = ((_width + 1) / 2);
		uint8_t* buf = (uint8_t*)_buffer;
		buf += bufBytewidth * y + x1 / 2;
		bool screen_alt_initial = (x1 % 2) == 0;
		
		
		uint8_t b1 = color.i;
		uint8_t b2 = color.iu;
		
		for (uint8_t dy=0; dy<h; dy++, bitmap+=bw, buf+=bufBytewidth) {
			const uint8_t* bitmap_ptr = bitmap;
			uint8_t bitmap_mask = first_bitmap_mask;
			uint8_t* screen_buf = buf;
			uint8_t pixels = *(bitmap_ptr++);
			bool screen_alt = screen_alt_initial;
			for (uint8_t sx=x1; sx<x2; sx++) {
				
				if (screen_alt) {
					if (pixels & bitmap_mask) {
						*screen_buf = (*screen_buf & 0x0F) | b2;
					}
				} else {
					if (pixels & bitmap_mask) {
						*screen_buf = (*screen_buf & 0xF0) | b1;
					}
					screen_buf++;
				}
				screen_alt = !screen_alt;
				
				bitmap_mask >>= 1;
				if (!bitmap_mask) {
					bitmap_mask = 0x80;
					pixels = *(bitmap_ptr++);
				}
			}
		}
	} else {
		uint8_t dw = 8 - (w%8);
		for (uint8_t j = 0; j < h; j++) {
			x = _x;
			for (uint8_t i = 0; i < bw;) {
				uint8_t b = *(bitmap++);
				i++;
				for (uint8_t k = 0; k < 8; k++) {
					if (i == bw && k == dw) {
						x += (w%8);
						break;
					}
					if (b&0x80) {
						drawPixel(x, y);
					}
					b <<= 1;
					x++;
				}
			}
			y++;
		}
	}
}

void Image::drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap,
	uint8_t rotation, uint8_t flip) {
	Graphics::drawBitmap(x, y, bitmap, rotation, flip);
}

void Image::drawImage(int16_t x, int16_t y, Image& img) {
	//draw INDEX => RGB, non-transparent
	if ((img.colorMode == ColorMode::index) && (colorMode == ColorMode::rgb565) && !img.useTransparentIndex) {
		img.nextFrame();
		int16_t w1 = img._width; //width of the source image
		int16_t h1 = img._height; //height of the source image
		if ((x > _width) || ((x + w1) < 0) || (y > _height) || ((y + h1) < 0)) return;
		//horizontal cropping
		int16_t i2offset = 0;
		int16_t w2cropped = w1;
		if (x < 0) {
			i2offset = -x;
			w2cropped = w1 + x;
			if (w2cropped > _width) {
				w2cropped = _width;
			}
		} else if ((x + w1) > _width) {
			w2cropped = _width - x;
		}

		//vertical cropping
		int16_t j2offset = 0;
		int16_t h2cropped = h1;
		if (y < 0) {
			j2offset = -y;
			h2cropped = h1 + y;
			if (h2cropped > _height) {
				h2cropped = _height;
			}
		} else if ((y + h1) > _height) {
			h2cropped = _height - y;
		}
		
		for (int j2 = 0; j2 < h2cropped; j2++) {
			uint8_t *srcLine;
			
			// w1+1 for ceiling rather than flooring
			srcLine = (uint8_t*)img._buffer + ((w1 + 1) / 2) * (j2 + j2offset) + (i2offset/2);
			

			indexTo565(
				&_buffer[x + i2offset + (y + j2offset + j2)*_width],
				srcLine, colorIndex, w2cropped, i2offset%2
			);
		}
		
		return;
	}
	Graphics::drawImage(x, y, img);
}

} // namespace Gamebuino_Meta
