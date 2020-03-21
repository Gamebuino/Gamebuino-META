/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "Graphics.h"
#include "Image.h"
#include "../Misc.h"

// default 3x5 font table
extern const uint8_t font3x5[];

// gb is only needed to check for the inited state to get proper width() and height() during initialization
#include "../../Gamebuino-Meta.h"

#ifndef min
#define min(x, y) ((x < y) ? x : y)
#endif

namespace Gamebuino_Meta {

extern Gamebuino* gbptr;

//default values of static members
//uint16_t Graphics::transparentColor = 0xF81F; //magenta is the default transparent color
uint16_t Graphics::tint = 0xFFFF;
uint8_t Graphics::alpha = 255;
const Color defaultColorPalette[16] = {
	Color::black,
	Color::darkblue,
	Color::purple,
	Color::green,
	Color::brown,
	Color::darkgray,
	Color::gray,
	Color::white,
	Color::red,
	Color::orange,
	Color::yellow,
	Color::lightgreen,
	Color::lightblue,
	Color::blue,
	Color::pink,
	Color::beige,
};
Color* Graphics::colorIndex = (Color*)defaultColorPalette;
BlendMode Graphics::blendMode = BlendMode::blend;
Graphics::ColorUnion Graphics::color = {(Color)0};
Graphics::BgcolorUnion Graphics::bgcolor = {(Color)0};
int16_t Graphics::cursorX = 0;
int16_t Graphics::cursorY = 0;
uint8_t Graphics::fontSize = 0;

bool Graphics::textWrap = true;
bool Graphics::_cp437 = false;
GFXfont* Graphics::gfxFont = 0;
uint8_t* Graphics::font = 0;
uint8_t Graphics::fontWidth = 0;
uint8_t Graphics::fontHeight = 0;

void Graphics::indexTo565(uint16_t *dest, uint8_t *src, Color *index, uint16_t length, bool skipFirst) {
	// length is the number of destination pixels
	// +1 for cieling rather than flooring
	uint8_t b = *(src++);
	for (uint16_t i = 0; i < length; i++) {
		if (skipFirst) {
			dest[i] = (uint16_t)index[b&0x0F];
			b = *(src++);
		} else {
			dest[i] = (uint16_t)index[b >> 4];
		}
		skipFirst = !skipFirst;
	}
}

ColorIndex Graphics::rgb565ToIndex(Color rgb) {
	for (uint8_t i = 0; i < 16; i++) {
		if (rgb == colorIndex[i]) {
			return (ColorIndex)i;
		}
	}
	// ok not part of the index, let's try to find the closest match!
	uint8_t b = (uint8_t)((uint16_t)rgb << 3);
	uint8_t g = (uint8_t)(((uint16_t)rgb >> 3) & 0xFC);
	uint8_t r = (uint8_t)(((uint16_t)rgb >> 8) & 0xF8);
	
	uint8_t min_index = 0;
	uint16_t min_diff = 0xFFFF;
	uint8_t i = 0;
	for (; i < 16; i++) {
		uint16_t rgb2 = (uint16_t)colorIndex[i];
		uint8_t b2 = (uint8_t)(rgb2 << 3);
		uint8_t g2 = (uint8_t)((rgb2 >> 3) & 0xFC);
		uint8_t r2 = (uint8_t)((rgb2 >> 8) & 0xF8);
		
		uint16_t diff = abs(b - b2) + abs(g - g2) + abs(r - r2);
		if (diff < min_diff) {
			min_diff = diff;
			min_index = i;
		}
	}
	return (ColorIndex)min_index;
} 

// Many (but maybe not all) non-AVR board installs define macros
// for compatibility with existing PROGMEM-reading AVR code.
// Do our own checks and defines here for good measure...

#ifndef pgm_read_byte
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
 #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
 #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
 #define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
 #define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

Graphics::Graphics(int16_t w, int16_t h) {
	_width    = w;
	_height   = h;
	cursorY  = cursorX    = 0;
	fontSize  = 1;
	setColor(Color::white);
	textWrap      = true;
	_cp437    = false;
	gfxFont   = NULL;
	colorMode = ColorMode::rgb565;
	setFont(font3x5);
}

Graphics::~Graphics() {
	
}

// Draw a circle outline
void Graphics::drawCircle(int16_t x0, int16_t y0, int16_t r) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	drawPixel(x0  , y0+r);
	drawPixel(x0  , y0-r);
	drawPixel(x0+r, y0  );
	drawPixel(x0-r, y0  );

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixel(x0 + x, y0 + y);
		drawPixel(x0 - x, y0 + y);
		drawPixel(x0 + x, y0 - y);
		drawPixel(x0 - x, y0 - y);
		drawPixel(x0 + y, y0 + x);
		drawPixel(x0 - y, y0 + x);
		drawPixel(x0 + y, y0 - x);
		drawPixel(x0 - y, y0 - x);
	}
}

void Graphics::drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername) {
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {
			drawPixel(x0 + x, y0 + y);
			drawPixel(x0 + y, y0 + x);
		}
		if (cornername & 0x2) {
			drawPixel(x0 + x, y0 - y);
			drawPixel(x0 + y, y0 - x);
		}
		if (cornername & 0x8) {
			drawPixel(x0 - y, y0 + x);
			drawPixel(x0 - x, y0 + y);
		}
		if (cornername & 0x1) {
			drawPixel(x0 - y, y0 - x);
			drawPixel(x0 - x, y0 - y);
		}
	}
}

void Graphics::fillCircle(int16_t x0, int16_t y0, int16_t r) {
	drawFastVLine(x0, y0-r, 2*r+1);
	fillCircleHelper(x0, y0, r, 3, 0);
}

// Used to do circles and roundrects
void Graphics::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
 uint8_t cornername, int16_t delta) {

	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1) {
			drawFastVLine(x0+x, y0-y, 2*y+1+delta);
			drawFastVLine(x0+y, y0-x, 2*x+1+delta);
		}
		if (cornername & 0x2) {
			drawFastVLine(x0-x, y0-y, 2*y+1+delta);
			drawFastVLine(x0-y, y0-x, 2*x+1+delta);
		}
	}
}

// Bresenham's algorithm - thx wikpedia
void Graphics::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0<=x1; x0++) {
		if (steep) {
			drawPixel(y0, x0);
		} else {
			drawPixel(x0, y0);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

// Draw a rectangle
void Graphics::drawRect(int16_t x, int16_t y, int16_t w, int16_t h) {
	drawFastHLine(x, y, w);
	drawFastHLine(x, y+h-1, w);
	drawFastVLine(x, y, h);
	drawFastVLine(x+w-1, y, h);
}

void Graphics::drawFastVLine(int16_t x, int16_t y, int16_t h) {
	// Update in subclasses if desired!
	if (h < 0) {
		y += h + 1;
		h = abs(h);
	}
	drawLine(x, y, x, y+h-1);
}

void Graphics::drawFastHLine(int16_t x, int16_t y, int16_t w) {
	// Update in subclasses if desired!
	if (w < 0) {
		x += w + 1;
		w = abs(w);
	}
	drawLine(x, y, x+w-1, y);
}

void Graphics::fillRect(int16_t x, int16_t y, int16_t w, int16_t h) {
	// Update in subclasses if desired!
	for (int16_t i=x; i<x+w; i++) {
		drawFastVLine(i, y, h);
	}
}

void Graphics::_fill() {
	fillRect(0, 0, _width, _height);
}

void Graphics::fill() {
	_fill();
}

void Graphics::fill(Color c) {
	Color tmpColor = setTmpColor(c);
	fill();
	color.c = tmpColor;
}

void Graphics::clearTextVars() {
	setCursor(0, 0);
	setFont(font3x5);
	fontSize = DEFAULT_FONT_SIZE;
	textWrap = true;
}

void Graphics::clear() {
	clear(Color::black);
}

void Graphics::clear(Color c) {
	if (colorMode == ColorMode::index) {
		clear(rgb565ToIndex(c));
		return;
	}
	fill(c);
	setColor((Color)(0xFFFF ^ (uint16_t)c));
	clearTextVars();
}

void Graphics::clear(ColorIndex c) {
	const ColorIndex complimentary[] = {
		ColorIndex::white, // black
		ColorIndex::yellow, // darkblue
		ColorIndex::lightgreen, // purple
		ColorIndex::orange, // green
		ColorIndex::darkgray, // brown
		ColorIndex::brown, // darkgray
		ColorIndex::red, // gray
		ColorIndex::black, // white
		ColorIndex::gray, // red
		ColorIndex::green, // orange
		ColorIndex::darkblue, // yellow
		ColorIndex::purple, // lightgreen
		ColorIndex::pink, // lightblue
		ColorIndex::beige, // blue
		ColorIndex::lightblue, // pink
		ColorIndex::blue, // beige
	};
	if (colorMode == ColorMode::rgb565) {
		clear(colorIndex[(uint8_t)c]);
		return;
	}
	fill(c);
	setColor(complimentary[(uint8_t)c]);
	clearTextVars();
}

void Graphics::fill(ColorIndex c) {
	Color tmpColor = setTmpColor(c);
	fill();
	color.c = tmpColor;
}

// Draw a rounded rectangle
void Graphics::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r) {
	// smarter version
	drawFastHLine(x+r  , y    , w-2*r); // Top
	drawFastHLine(x+r  , y+h-1, w-2*r); // Bottom
	drawFastVLine(x    , y+r  , h-2*r); // Left
	drawFastVLine(x+w-1, y+r  , h-2*r); // Right
	// draw four corners
	drawCircleHelper(x+r    , y+r    , r, 1);
	drawCircleHelper(x+w-r-1, y+r    , r, 2);
	drawCircleHelper(x+w-r-1, y+h-r-1, r, 4);
	drawCircleHelper(x+r    , y+h-r-1, r, 8);
}

// Fill a rounded rectangle
void Graphics::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r) {
	// smarter version
	fillRect(x+r, y, w-2*r, h);

	// draw four corners
	fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1);
	fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1);
}

// Draw a triangle
void Graphics::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	drawLine(x0, y0, x1, y1);
	drawLine(x1, y1, x2, y2);
	drawLine(x2, y2, x0, y0);
}

// Fill a triangle
void Graphics::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2) {

	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		_swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
	}
	if (y1 > y2) {
		_swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
	}
	if (y0 > y1) {
		_swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
	}

	if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if(x1 < a)      a = x1;
		else if(x1 > b) b = x1;
		if(x2 < a)      a = x2;
		else if(x2 > b) b = x2;
		drawFastHLine(a, y0, b-a+1);
		return;
	}

	int16_t
		dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1;
	int32_t
		sa   = 0,
		sb   = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if(y1 == y2) last = y1;   // Include y1 scanline
	else         last = y1-1; // Skip it

	for(y=y0; y<=last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b) _swap_int16_t(a,b);
		drawFastHLine(a, y, b-a+1);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b) _swap_int16_t(a,b);
		drawFastHLine(a, y, b-a+1);
	}
}

void Graphics::drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap) {
	uint8_t w = *(bitmap++);
	uint8_t h = *(bitmap++);
	
	uint8_t byteWidth = (w + 7) / 8;
	uint8_t _x = x;
	uint8_t dw = 8 - (w%8);
	for (uint8_t j = 0; j < h; j++) {
		x = _x;
		for (uint8_t i = 0; i < byteWidth;) {
			uint8_t b = *(bitmap++);
			i++;
			for (uint8_t k = 0; k < 8; k++) {
				if (i == byteWidth && k == dw) {
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

void Graphics::drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap, int8_t scale) {
	if (scale == 1) {
		drawBitmap(x, y, bitmap);
	}
	uint8_t w = *(bitmap++);
	uint8_t h = *(bitmap++);
	
	uint8_t byteWidth = (w + 7) / 8;
	uint8_t _x = x;
	uint8_t dw = 8 - (w%8);
	for (uint8_t j = 0; j < h; j++) {
		x = _x;
		for (uint8_t i = 0; i < byteWidth;) {
			uint8_t b = *(bitmap++);
			i++;
			for (uint8_t k = 0; k < 8; k++) {
				if (i == byteWidth && k == dw) {
					x += (w%8)*scale;
					break;
				}
				if (b&0x80) {
					fillRect(x, y, scale, scale);
				}
				b <<= 1;
				x += scale;
			}
		}
		y += scale;
	}
}

void Graphics::drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap,
	uint8_t rotation, uint8_t flip) {
	if ((rotation == NOROT) && (flip == NOFLIP)) {
		drawBitmap(x, y, bitmap); //use the faster algorithm
		return;
	}
	uint8_t w = pgm_read_byte(bitmap);
	uint8_t h = pgm_read_byte(bitmap + 1);
	bitmap = bitmap + 2; //add an offset to the pointer to start after the width and height
	int8_t i, j, //coordinates in the raw bitmap
			k, l, //coordinates in the rotated/flipped bitmap
			byteNum, bitNum, byteWidth = (w + 7) >> 3;

	rotation %= 4;

	for (i = 0; i < w; i++) {
		byteNum = i / 8;
		bitNum = i % 8;
		for (j = 0; j < h; j++) {
			if (pgm_read_byte(bitmap + j * byteWidth + byteNum) & (0b10000000 >> bitNum)) {
				switch (rotation) {
				case NOROT: //no rotation
					k = i;
					l = j;
					break;
				case ROTCCW: //90° counter-clockwise
					k = j;
					l = w - i - 1;
					break;
				case ROT180: //180°
					k = w - i - 1;
					l = h - j - 1;
					break;
				case ROTCW: //90° clockwise
					k = h - j - 1;
					l = i;
					break;
				}
				if (flip) {
					if (flip & 0b00000001) { //horizontal flip
						k = w - k - 1;
					}
					if (flip & 0b00000010) { //vertical flip
						l = h - l;
					}
				}
				k += x; //place the bitmap on the screen
				l += y;
				drawPixel(k, l);
			}
		}
	}
}


bool Graphics::getBitmapPixel(const uint8_t* bitmap, uint8_t x, uint8_t y) {
	return pgm_read_byte(bitmap + 2 + y * ((pgm_read_byte(bitmap) + 7) / 8) + (x >> 3)) & (0b10000000 >> (x % 8));
}

void Graphics::drawImage(int16_t x, int16_t y, Image& img) {
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
	
	drawImageCrop(x, y, w1, i2offset, w2cropped, j2offset, h2cropped, img);
}

void Graphics::drawImage(int16_t x, int16_t y, Image& img, int16_t w2, int16_t h2) {
	img.nextFrame();
	if ((x > _width) || ((x + abs(w2)) < 0) || (y > _height) || ((y + abs(h2)) < 0) || (w2 == 0) || (h2 == 0)) return;

	int16_t w = img._width;
	int16_t h = img._height;

	//no scaling, fall back to the regular function
	if ((w == w2) && (h == h2)) {
		drawImage(x, y, img);
		return;
	}

	bool invertX = (w2 < 0);
	bool invertY = (h2 < 0);
	w2 = abs(w2);
	h2 = abs(h2);


	//horizontal cropping
	int16_t w2cropped = w2; //width of the cropped buffer
	int16_t i2offset = 0;
	if (x < 0) {
		i2offset = -x;
		w2cropped = w2 + x;
	} else if ((x + w2) > _width) {
		w2cropped = _width - x;
	}

	//vertical cropping
	int16_t h2cropped = h2;
	int16_t j2offset = 0;
	if (y < 0) {
		j2offset = -y;
		h2cropped = h2 + y;
	} else if ((y + h2) > _height) {
		h2cropped = _height - y;
	}
	
	if (colorMode == ColorMode::rgb565) {
		uint16_t bufferLine[w2];
		uint16_t transparent_backup = img.transparentColor;
		if (img.colorMode == ColorMode::index) {
			if (img.useTransparentIndex) {
				img.transparentColor = (uint16_t)colorIndex[img.transparentColorIndex];
			} else {
				img.transparentColor = 1;
			}
			
		}
		for (int16_t j2 = 0; j2 < h2cropped; j2++) { //j2: offseted vertical coordinate in destination
			uint16_t j = h * (j2 + j2offset) / h2; //j: vertical coordinate in source image
			j = invertY ? h - 1 - j : j;
			for (int16_t i2 = 0; i2 < w2cropped; i2++) { //i2: offseted horizontal coordinate in desination
				uint16_t i = w * (i2 + i2offset) / w2; //i: horizontal coordinate in original image
				i = invertX ? w - 1 - i : i;
				if (img.colorMode == ColorMode::rgb565) {
					// draw RGB => RGB
					bufferLine[i2] = img._buffer[(j * w) + i];
				} else if (img.colorMode == ColorMode::index) {
					//draw INDEX => RGB
					uint8_t b = ((uint8_t*)img._buffer)[((w+1)/2)*j + (i/2)];
					if (!(i % 2)) {
						b >>= 4;
					}
					bufferLine[i2] = (uint16_t)colorIndex[b & 0x0F];
				}
			}
			drawBufferedLine(x + i2offset, y + j2 + j2offset, bufferLine, w2cropped, img);
		}
		img.transparentColor = transparent_backup;
		return;
	}
	if (colorMode == ColorMode::index) {
		if (img.colorMode == ColorMode::rgb565) {
			return;
		}
		uint8_t bufferLine[w2 + 1];
		for (int16_t j2 = 0; j2 < h2cropped; j2++) { //j2: offseted vertical coordinate in destination
			uint8_t* dst = bufferLine;
			uint16_t j = h * (j2 + j2offset) / h2; //j: vertical coordinate in source image
			j = invertY ? h - 1 - j : j;
			for (int16_t i2 = 0; i2 < w2cropped; i2++) { //i2: offseted horizontal coordinate in desination
				uint16_t i = w * (i2 + i2offset) / w2; //i: horizontal coordinate in original image
				i = invertX ? w - 1 - i : i;
				
				//draw INDEX => INDEX
				uint8_t b = ((uint8_t*)img._buffer)[((w+1)/2)*j + (i/2)];
				if (!(i % 2)) {
					b >>= 4;
				}
				b &= 0x0F;
				if ((x + i2 + i2offset) % 2) {
					*dst = (*dst & 0xF0) | b;
					dst++;
				} else {
					*dst = (*dst & 0x0F) | (b << 4);
				}
			}
			drawBufferedLine(x + i2offset, y + j2 + j2offset, (uint16_t*)bufferLine, w2cropped, img);
		}
		return;
	}
}

void Graphics::drawImage(int16_t x, int16_t y, Image& img, int16_t x2, int16_t y2, int16_t w2, int16_t h2) {
	img.nextFrame();
	int16_t w1 = img._width; //width of the source image
	int16_t h1 = img._height; //height of the source image
	if (w2 == 0 || w1 == 0 || (x > _width) || ((x + w1) < 0) || (y > _height) || ((y + h1) < 0)) return;

	//no scaling, fall back to the regular function
	if ((x2 == 0) && (y2 == 0) && (w1 == w2) && (h1 == h2)) {
		drawImage(x, y, img);
		return;
	}
	w2 = min(w2, w1 - x2);
	h2 = min(h2, h1 - y2);
	
	//horizontal cropping
	int16_t i2offset = x2;
	int16_t w2cropped = w2;
	if (x < 0) {
		i2offset = x2 - x;
		w2cropped = w2 + x;
		if (w2cropped > _width) {
			w2cropped = _width;
		}
	} else if ((x + w2) > _width) {
		w2cropped = _width - x;
	}
	x -= x2;
	
	//vertical cropping
	int16_t j2offset = y2;
	int16_t h2cropped = h2;
	if (y < 0) {
		j2offset = y2 - y;
		h2cropped = h2 + y;
		if (h2cropped > _height) {
			h2cropped = _height;
		}
	} else if ((y + h2) > _height) {
		h2cropped = _height - y;
	}
	y -= y2;
	
	drawImageCrop(x, y, w1, i2offset, w2cropped, j2offset, h2cropped, img);
}

void Graphics::drawImageCrop(int16_t x, int16_t y, int16_t w1, int16_t i2offset, int16_t w2cropped, int16_t j2offset, int16_t h2cropped, Image& img) {
	//draw INDEX => RGB
	if ((img.colorMode == ColorMode::index) && (colorMode == ColorMode::rgb565)) {
		uint16_t transparent_backup = img.transparentColor;
		if (img.useTransparentIndex) {
			img.transparentColor = (uint16_t)colorIndex[img.transparentColorIndex];
		} else {
			img.transparentColor = 1;
		}
		for (int j2 = 0; j2 < h2cropped; j2++) {
			uint16_t destLineArray[w2cropped];
			uint16_t *destLine = destLineArray;
			uint8_t *srcLine;
			
			// w1+1 for ceiling rather than flooring
			srcLine = (uint8_t*)img._buffer + ((w1 + 1) / 2) * (j2 + j2offset) + (i2offset/2);
			
//			srcLine = (uint8_t*)img._buffer + (((j2 + j2offset) * w1) + i2offset) / 2;

			indexTo565(destLine, srcLine, colorIndex, w2cropped, i2offset%2);

			/*for (uint16_t i = 0; i < w2cropped/2; i++) { //horizontal coordinate in source image
				uint16_t color = destLine[i];
				color = (color << 8) | (color >> 8); //change endianness
				destLine[i] = color;
			}*/

			drawBufferedLine(
				x + i2offset,
				y + j2offset + j2,
				destLine,
				w2cropped,
				img
			);
		}
		img.transparentColor = transparent_backup;
		return;
	}

	//draw RGB => RGB
	if ((img.colorMode == ColorMode::rgb565) && (colorMode == ColorMode::rgb565)) {
		for (int j2 = 0; j2 < h2cropped; j2++) { //j2 : offseted vertical coordinate in the destination image
			drawBufferedLine(
				x + i2offset,
				y + j2offset + j2,
				img._buffer + ((j2 + j2offset) * w1) + i2offset,
				w2cropped,
				img
			);
		}
		return;
	}

	//draw INDEX => INDEX
	if ((img.colorMode == ColorMode::index) && (colorMode == ColorMode::index)) {
		for (int j2 = 0; j2 < h2cropped; j2++) {
			
			if (x % 2) {
				uint8_t *src = (uint8_t*)img._buffer + ((j2 + j2offset) * ((w1 + 1)/2)) + ((i2offset + 1) / 2);
				uint8_t destLineArray[((w2cropped+1) / 2) + 1];
				uint8_t* destLine = destLineArray;
				uint8_t b;
				uint8_t b_2 = src[-1];
				for (uint8_t i = 0; i < ((w2cropped + 1) / 2) + 1; i++) {
					b = src[i];
					destLine[i] = (b >> 4) | (b_2 << 4);
					b_2 = b;
				}
				drawBufferedLine(
					x + i2offset,
					y + j2offset + j2,
					(uint16_t*)destLine,
					w2cropped,
					img
				);
			} else {
				uint8_t *destLine = (uint8_t*)img._buffer + ((j2 + j2offset) * ((w1 + 1)/2)) + (i2offset / 2);
				drawBufferedLine(
					x + i2offset,
					y + j2offset + j2,
					(uint16_t*)destLine,
					w2cropped,
					img
				);
			}
		}
	}

	/*//the dumb way
	for (int i = 0; i < img._width; i++) {
		for (int j = 0; j < img._height; j++) {
			setColor(img._buffer[i + img._width*j]);
			drawPixel(x + i, y + j);
		}
	}*/
}


#if ARDUINO >= 100
size_t Graphics::write(uint8_t c) {
#else
void Graphics::write(uint8_t c) {
#endif

	if(!gfxFont) { // 'Classic' built-in font

		if(c == '\n') {
			cursorY += fontSize*fontHeight;
			if(textWrap && ((cursorY + fontSize * fontHeight) >= _height)) {//Heading off bottom edge ?
				cursorY = 0;
			}
			cursorX  = 0;
		} else if(c == '\r') {
			// skip em
		} else {
			if(textWrap && ((cursorX + fontSize * fontWidth) > _width)) { // Heading off right edge?
				cursorX  = 0;            // Reset x to zero
				cursorY += fontSize * fontHeight; // Advance y one line
			}
			drawChar(cursorX, cursorY, c, fontSize);
			cursorX += fontSize * fontWidth;
		}

	} else { // Custom font

		if(c == '\n') {
			cursorX  = 0;
			cursorY += (int16_t)fontSize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
		} else if(c != '\r') {
			uint8_t first = pgm_read_byte(&gfxFont->first);
			if((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
				uint8_t   c2    = c - pgm_read_byte(&gfxFont->first);
				GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c2]);
				uint8_t   w     = pgm_read_byte(&glyph->width),
						h     = pgm_read_byte(&glyph->height);
				if((w > 0) && (h > 0)) { // Is there an associated bitmap?
					int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
					if(textWrap && ((cursorX + fontSize * (xo + w)) >= _width)) {
						// Drawing character would go off right edge; textWrap to new line
						cursorX  = 0;
						cursorY += (int16_t)fontSize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
					}
					drawChar(cursorX, cursorY, c, fontSize);
				}
				cursorX += pgm_read_byte(&glyph->xAdvance) * (int16_t)fontSize;
			}
		}

	}
#if ARDUINO >= 100
	return 1;
#endif
}

// Draw a character
void Graphics::drawChar(int16_t x, int16_t y, unsigned char c, uint8_t size) {
	if(!gfxFont) { // 'Classic' built-in font

		if((x >= _width)            || // Clip right
			 (y >= _height)           || // Clip bottom
			 ((x + fontWidth * size - 1) < 0) || // Clip left
			 ((y + fontHeight * size - 1) < 0))   // Clip top
			return;

		if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

		if (c >= 0x80) {
			c -= 0x20;
		}

		for (int8_t i = 0; i < fontWidth; i++) {
			uint8_t line;
			if (i == (fontWidth - 1))
				line = 0x0;
			else
				line = pgm_read_byte(font + (c * (fontWidth - 1)) + i);
			for (int8_t j = 0; j < fontHeight; j++) {
				if (line & 0x1) {
					if (size == 1) // default size
						drawPixel(x + i, y + j);
					else { // big size
						fillRect(x + (i * size), y + (j * size), size, size);
					}
				}
				else if (bgcolor.c != color.c) {
					Color tempcolor = color.c;
					color.c = bgcolor.c;
					if (size == 1) // default size
						drawPixel(x + i, y + j);
					else { // big size
						fillRect(x + i*size, y + j*size, size, size);
					}
					color.c = tempcolor; //restore color to its initial value
				}
				line >>= 1;
			}
		}

	} else { // Custom font

		// Character is assumed previously filtered by write() to eliminate
		// newlines, returns, non-printable characters, etc.  Calling drawChar()
		// directly with 'bad' characters of font may cause mayhem!

		c -= pgm_read_byte(&gfxFont->first);
		GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
		uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

		uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
		uint8_t  w  = pgm_read_byte(&glyph->width),
					 h  = pgm_read_byte(&glyph->height);
		int8_t   xo = pgm_read_byte(&glyph->xOffset),
					 yo = pgm_read_byte(&glyph->yOffset);
		uint8_t  xx, yy, bits, bit = 0;
		int16_t  xo16, yo16;

		if(size > 1) {
			xo16 = xo;
			yo16 = yo;
		}

		// Todo: Add character clipping here

		// NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
		// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
		// has typically been used with the 'classic' font to overwrite old
		// screen contents with new data.  This ONLY works because the
		// characters are a uniform size; it's not a sensible thing to do with
		// proportionally-spaced fonts with glyphs of varying sizes (and that
		// may overlap).  To replace previously-drawn text when using a custom
		// font, use the getTextBounds() function to determine the smallest
		// rectangle encompassing a string, erase the area with fillRect(),
		// then draw new text.  This WILL infortunately 'blink' the text, but
		// is unavoidable.  Drawing 'background' pixels will NOT fix this,
		// only creates a new set of problems.  Have an idea to work around
		// this (a canvas object type for MCUs that can afford the RAM and
		// displays supporting setAddrWindow() and pushColors()), but haven't
		// implemented this yet.

		for(yy=0; yy<h; yy++) {
			for(xx=0; xx<w; xx++) {
				if(!(bit++ & 7)) {
					bits = pgm_read_byte(&bitmap[bo++]);
				}
				if(bits & 0x80) {
					if(size == 1) {
						drawPixel(x+xo+xx, y+yo+yy);
					} else {
						fillRect(x+(xo16+xx)*size, y+(yo16+yy)*size, size, size);
					}
				}
				bits <<= 1;
			}
		}

	} // End classic vs custom font
}

int16_t Graphics::getCursorX(void) const {
	return cursorX;
}

int16_t Graphics::getCursorY(void) const {
	return cursorY;
}

uint8_t Graphics::getFontWidth(void) const {
	return fontWidth;
}

uint8_t Graphics::getFontHeight(void) const {
	return fontHeight;
}

void Graphics::setFontSize(uint8_t s) {
	fontSize = (s > 0) ? s : 1;
}

void Graphics::drawPixel(int16_t x, int16_t y) {
	_drawPixel(x, y);
}

void Graphics::drawPixel(int16_t x, int16_t y, Color c) {
	Color tmpColor = setTmpColor(c);
	drawPixel(x, y);
	color.c = tmpColor;
}

void Graphics::drawPixel(int16_t x, int16_t y, ColorIndex c) {
	Color tmpColor = setTmpColor(c);
	drawPixel(x, y);
	color.c = tmpColor;
}

Color Graphics::setTmpColor(Color c) {
	if (colorMode == ColorMode::index) {
		return setTmpColor(rgb565ToIndex(c));
	}
	Color tmpColor = color.c;
	color.c = c;
	return tmpColor;
}

Color Graphics::setTmpColor(ColorIndex c) {
	if (colorMode == ColorMode::index) {
		Color tmpColor = color.c;
		color.i = (uint8_t)c & 0x0F;
		color.iu = color.i << 4;
		return tmpColor;
	}
	return setTmpColor(colorIndex[(uint8_t)c]);
}

void Graphics::setColor(Color c) {
	// For 'transparent' background, we'll set the bg
	// to the same as fg instead of using a flag
	if (colorMode == ColorMode::index) {
		setColor(rgb565ToIndex(c));
	} else {
		color.c = bgcolor.c = c;
	}
}

void Graphics::setColor(Color c, Color b) {
	if (colorMode == ColorMode::index) {
		setColor(rgb565ToIndex(c), rgb565ToIndex(b));
	} else {
		color.c = c;
		bgcolor.c = b;
	}
}

void Graphics::setColor(ColorIndex c) {
	if (colorMode == ColorMode::index) {
		color.i = (uint8_t)c & 0x0F;
		color.iu = color.i << 4;
		bgcolor.c = color.c; // simpler to just have them be the same
	} else {
		setColor(colorIndex[(uint8_t)c]);
	}
}
void Graphics::setColor(ColorIndex c, ColorIndex bg) {
	if (colorMode == ColorMode::index) {
		color.i = (uint8_t)c & 0x0F;
		color.iu = color.i << 4;
		bgcolor.i = (uint8_t)bg & 0x0F;
		bgcolor.iu = bgcolor.i << 4;
	} else {
		setColor(colorIndex[(uint8_t)c], colorIndex[(uint8_t)bg]);
	}
}

void Graphics::setColor(uint8_t c) {
	setColor((ColorIndex)c);
}

void Graphics::setColor(uint8_t c, uint8_t bg) {
	setColor((ColorIndex)c, (ColorIndex)bg);
}

void Graphics::setColor(uint8_t r, uint8_t g, uint8_t b) {
	setColor((Color)rgb888Torgb565({r, g, b}));
}

void Graphics::setTransparentColor(Color c) {
	if (colorMode == ColorMode::index) {
		useTransparentIndex = true;
		transparentColorIndex = (uint8_t)rgb565ToIndex(c);
	} else {
		transparentColor = (uint16_t)c;
	}
}

void Graphics::setTransparentColor(ColorIndex c) {
	if (colorMode == ColorMode::index) {
		useTransparentIndex = true;
		transparentColorIndex = (uint8_t)c;
	} else {
		setTransparentColor(colorIndex[(uint8_t)c]);
	}
}

void Graphics::clearTransparentColor() {
	if (colorMode == ColorMode::index) {
		useTransparentIndex = false;
	} else {
		transparentColor = 0;
	}
}

void Graphics::setCursorX(int16_t x) {
	cursorX = x;
}

void Graphics::setCursorY(int16_t y) {
	cursorY = y;
}

void Graphics::setCursor(int16_t x, int16_t y) {
	cursorX = x;
	cursorY = y;
}

void Graphics::setTextWrap(bool w) {
	textWrap = w;
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void Graphics::cp437(bool x) {
	_cp437 = x;
}

//adafruit custom font
void Graphics::setFont(const GFXfont *f) {
	if(f) {          // Font struct pointer passed in?
		if(!gfxFont) { // And no current font struct?
			// Switching from classic to new font behavior.
			// Move cursor pos down 6 pixels so it's on baseline.
			cursorY += 6;
		}
	} else if(gfxFont) { // NULL passed.  Current font struct defined?
		// Switching from new to classic font behavior.
		// Move cursor pos up 6 pixels so it's at top-left of char.
		cursorY -= 6;
	}
	gfxFont = (GFXfont *)f;
}

//gamebuino legacy font
void Graphics::setFont(const uint8_t *f) {
	font = (uint8_t*)f;
	fontWidth = pgm_read_byte(font) + 1;
	fontHeight = pgm_read_byte(font + 1) + 1;
	font += 2; //offset the pointer to start after the width and height bytes
	_cp437 = true; // disable the old compatibility mode
}
// Pass string, returns W,H.
void Graphics::getTextBounds(const char *str, uint16_t *w, uint16_t *h) {
	uint8_t c; // Current character
	int16_t x = 0;
	int16_t y = 0;
	*w  = *h = 0;

	if(gfxFont) {

		GFXglyph *glyph;
		uint8_t   first = pgm_read_byte(&gfxFont->first),
				last  = pgm_read_byte(&gfxFont->last),
				gw, gh, xa;
		int8_t    xo, yo;
		int16_t   minx = _width, miny = _height, maxx = -1, maxy = -1,
				gx1, gy1, gx2, gy2, ts = (int16_t)fontSize,
				ya = ts * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);

		while((c = *str++)) {
			if(c != '\n') { // Not a newline
				if(c != '\r') { // Not a carriage return, is normal char
					if((c >= first) && (c <= last)) { // Char present in current font
						c    -= first;
						glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
						gw    = pgm_read_byte(&glyph->width);
						gh    = pgm_read_byte(&glyph->height);
						xa    = pgm_read_byte(&glyph->xAdvance);
						xo    = pgm_read_byte(&glyph->xOffset);
						yo    = pgm_read_byte(&glyph->yOffset);
						if(textWrap && ((x + (((int16_t)xo + gw) * ts)) >= _width)) {
							// Line textWrap
							x  = 0;  // Reset x to 0
							y += ya; // Advance y by 1 line
						}
						gx1 = x   + xo * ts;
						gy1 = y   + yo * ts;
						gx2 = gx1 + gw * ts - 1;
						gy2 = gy1 + gh * ts - 1;
						if(gx1 < minx) minx = gx1;
						if(gy1 < miny) miny = gy1;
						if(gx2 > maxx) maxx = gx2;
						if(gy2 > maxy) maxy = gy2;
						x += xa * ts;
					}
				} // Carriage return = do nothing
			} else { // Newline
				x  = 0;  // Reset x
				y += ya; // Advance y by 1 line
			}
		}
		// End of string
		if(maxx >= minx) *w  = maxx - minx + 1;
		if(maxy >= miny) *h  = maxy - miny + 1;

	} else { // Default font

		uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

		while((c = *str++)) {
			if(c != '\n') { // Not a newline
				if(c != '\r') { // Not a carriage return, is normal char
					if(textWrap && ((x + fontSize * 6) >= _width)) {
						x  = 0;            // Reset x to 0
						y += fontSize * fontHeight; // Advance y by 1 line
						if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
						lineWidth  = fontSize * fontWidth; // First char on new line
					} else { // No line textWrap, just keep incrementing X
						lineWidth += fontSize * fontWidth; // Includes interchar x gap
					}
				} // Carriage return = do nothing
			} else { // Newline
				x  = 0;            // Reset x to 0
				y += fontSize * 8; // Advance y by 1 line
				if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
				lineWidth = 0;     // Reset lineWidth for new line
			}
		}
		// End of string
		if(lineWidth) y += fontSize * 8; // Add height of last (or only) line
		if(lineWidth > maxWidth) maxWidth = lineWidth; // Is the last or only line the widest?
		*w = maxWidth - 1;               // Don't include last interchar x gap
		*h = y;

	} // End classic vs custom font
}
// Pass string and a cursor position, returns UL corner and W,H.
void Graphics::getTextBounds(char *str, int16_t x, int16_t y,
 int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
	uint8_t c; // Current character

	*x1 = x;
	*y1 = y;
	*w  = *h = 0;

	if(gfxFont) {

		GFXglyph *glyph;
		uint8_t   first = pgm_read_byte(&gfxFont->first),
				last  = pgm_read_byte(&gfxFont->last),
				gw, gh, xa;
		int8_t    xo, yo;
		int16_t   minx = _width, miny = _height, maxx = -1, maxy = -1,
				gx1, gy1, gx2, gy2, ts = (int16_t)fontSize,
				ya = ts * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);

		while((c = *str++)) {
			if(c != '\n') { // Not a newline
				if(c != '\r') { // Not a carriage return, is normal char
					if((c >= first) && (c <= last)) { // Char present in current font
						c    -= first;
						glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
						gw    = pgm_read_byte(&glyph->width);
						gh    = pgm_read_byte(&glyph->height);
						xa    = pgm_read_byte(&glyph->xAdvance);
						xo    = pgm_read_byte(&glyph->xOffset);
						yo    = pgm_read_byte(&glyph->yOffset);
						if(textWrap && ((x + (((int16_t)xo + gw) * ts)) >= _width)) {
							// Line textWrap
							x  = 0;  // Reset x to 0
							y += ya; // Advance y by 1 line
						}
						gx1 = x   + xo * ts;
						gy1 = y   + yo * ts;
						gx2 = gx1 + gw * ts - 1;
						gy2 = gy1 + gh * ts - 1;
						if(gx1 < minx) minx = gx1;
						if(gy1 < miny) miny = gy1;
						if(gx2 > maxx) maxx = gx2;
						if(gy2 > maxy) maxy = gy2;
						x += xa * ts;
					}
				} // Carriage return = do nothing
			} else { // Newline
				x  = 0;  // Reset x
				y += ya; // Advance y by 1 line
			}
		}
		// End of string
		*x1 = minx;
		*y1 = miny;
		if(maxx >= minx) *w  = maxx - minx + 1;
		if(maxy >= miny) *h  = maxy - miny + 1;

	} else { // Default font

		uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

		while((c = *str++)) {
			if(c != '\n') { // Not a newline
				if(c != '\r') { // Not a carriage return, is normal char
					if(textWrap && ((x + fontSize * 6) >= _width)) {
						x  = 0;            // Reset x to 0
						y += fontSize * fontHeight; // Advance y by 1 line
						if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
						lineWidth  = fontSize * fontWidth; // First char on new line
					} else { // No line textWrap, just keep incrementing X
						lineWidth += fontSize * fontWidth; // Includes interchar x gap
					}
				} // Carriage return = do nothing
			} else { // Newline
				x  = 0;            // Reset x to 0
				y += fontSize * 8; // Advance y by 1 line
				if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
				lineWidth = 0;     // Reset lineWidth for new line
			}
		}
		// End of string
		if(lineWidth) y += fontSize * 8; // Add height of last (or only) line
		if(lineWidth > maxWidth) maxWidth = lineWidth; // Is the last or only line the widest?
		*w = maxWidth - 1;               // Don't include last interchar x gap
		*h = y - *y1;

	} // End classic vs custom font
}

// Same as above, but for PROGMEM strings
void Graphics::getTextBounds(const char *str,
 int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
	return getTextBounds((char*)str, x, y, x1, y1, w, h);
}

// Return the size of the display
int16_t Graphics::width(void) const {
	if ((gbptr && gbptr->inited) || _width) {
		// we are inited
		return _width;
	}
	// we aren't inited, so let's try our best guess
#if DISPLAY_MODE == DISPLAY_MODE_INDEX
	return 160;
#else
	return 80;
#endif
}

int16_t Graphics::height(void) const {
	if ((gbptr && gbptr->inited) || _height) {
		// we are inited
		return _height;
	}
	// we aren't inited, so let's try our best guess
#if DISPLAY_MODE == DISPLAY_MODE_INDEX
	return 128;
#else
	return 64;
#endif
}

void Graphics::invertDisplay(bool i) {
	// Do nothing, must be subclassed if supported by hardware
}

void Graphics::setPalette(Color* p) {
	colorIndex = p;
}

void Graphics::setPalette(const Color* p) {
	colorIndex = (Color*)p;
}

Color* Graphics::getPalette() {
	return colorIndex;
}

} // namespace Gamebuino_Meta
