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

#ifdef __AVR__
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#endif
#include "Adafruit_GFX.h"
#include "glcdfont.c"

#include "Image.h"

//default values of static members
uint16_t Adafruit_GFX::transparentColor = 0xF81F; //magenta is the default transparent color
uint16_t Adafruit_GFX::tint = 0xFFFF;
uint8_t Adafruit_GFX::alpha = 255;
uint16_t _colorIndex[16] = {0x0,0x194A,0x792A,0x42A,0xAA86,0x62C9,0xC618,0xFF9D,0xE8CA,0xFD03,0xF765,0x5DC9,0x553B,0x83B3,0xF3B4,0xFE75 };
uint16_t* Adafruit_GFX::colorIndex = _colorIndex;
BlendMode Adafruit_GFX::blendMode = BlendMode::BLEND;

void Adafruit_GFX::indexTo565(uint16_t *dest, uint16_t *src, uint16_t *index, uint16_t length) {
	//length is the number of destination pixels
	for (uint16_t i = 0; i < length/2; i++) {
		uint8_t index1 = ((uint8_t*)src)[i] & 0x0F;
		uint8_t index2 = ((uint8_t*)src)[i] >> 4;
		dest[i * 2] = index[index1];
		dest[(i * 2) + 1] = index[index2];
	}
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

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

Adafruit_GFX::Adafruit_GFX(int16_t w, int16_t h):
  WIDTH(w), HEIGHT(h)
{
  _width    = WIDTH;
  _height   = HEIGHT;
  rotation  = 0;
  cursorY  = cursorX    = 0;
  textsize  = 1;
  color = bgcolor = 0xFFFF;
  wrap      = true;
  _cp437    = false;
  gfxFont   = NULL;
  colorMode = ColorMode::RGB565;
}

// Draw a circle outline
void Adafruit_GFX::drawCircle(int16_t x0, int16_t y0, int16_t r) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0  , y0+r, color);
  drawPixel(x0  , y0-r, color);
  drawPixel(x0+r, y0  , color);
  drawPixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void Adafruit_GFX::drawCircleHelper( int16_t x0, int16_t y0,
 int16_t r, uint8_t cornername) {
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
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void Adafruit_GFX::fillCircle(int16_t x0, int16_t y0, int16_t r) {
  drawFastVLine(x0, y0-r, 2*r+1);
  fillCircleHelper(x0, y0, r, 3, 0);
}

// Used to do circles and roundrects
void Adafruit_GFX::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
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
void Adafruit_GFX::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
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
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// Draw a rectangle
void Adafruit_GFX::drawRect(int16_t x, int16_t y, int16_t w, int16_t h) {
  drawFastHLine(x, y, w);
  drawFastHLine(x, y+h-1, w);
  drawFastVLine(x, y, h);
  drawFastVLine(x+w-1, y, h);
}

void Adafruit_GFX::drawFastVLine(int16_t x, int16_t y,
 int16_t h) {
  // Update in subclasses if desired!
  drawLine(x, y, x, y+h-1);
}

void Adafruit_GFX::drawFastHLine(int16_t x, int16_t y,
 int16_t w) {
  // Update in subclasses if desired!
  drawLine(x, y, x+w-1, y);
}

void Adafruit_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h) {
  // Update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++) {
    drawFastVLine(i, y, h);
  }
}

void Adafruit_GFX::fillScreen(uint16_t color) {
  fillRect(0, 0, _width, _height);
}

// Draw a rounded rectangle
void Adafruit_GFX::drawRoundRect(int16_t x, int16_t y, int16_t w,
 int16_t h, int16_t r) {
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
void Adafruit_GFX::fillRoundRect(int16_t x, int16_t y, int16_t w,
 int16_t h, int16_t r) {
  // smarter version
  fillRect(x+r, y, w-2*r, h);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1);
}

// Draw a triangle
void Adafruit_GFX::drawTriangle(int16_t x0, int16_t y0,
 int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
  drawLine(x0, y0, x1, y1);
  drawLine(x1, y1, x2, y2);
  drawLine(x2, y2, x0, y0);
}

// Fill a triangle
void Adafruit_GFX::fillTriangle(int16_t x0, int16_t y0,
 int16_t x1, int16_t y1, int16_t x2, int16_t y2) {

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

// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground (for set bits) and background (for clear bits) colors.
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y,
 const uint8_t *bitmap, int16_t w, int16_t h) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) drawPixel(x+i, y+j, color);
      else            drawPixel(x+i, y+j, bgcolor);
    }
  }
}

// drawBitmap() variant w/background for RAM-resident (not PROGMEM) bitmaps.
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y,
 uint8_t *bitmap, int16_t w, int16_t h) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) byte <<= 1;
      else      byte   = bitmap[j * byteWidth + i / 8];
      if(byte & 0x80) drawPixel(x+i, y+j, color);
      else            drawPixel(x+i, y+j, bgcolor);
    }
  }
}

void Adafruit_GFX::drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap) {
	int8_t w = pgm_read_byte(bitmap);
	int8_t h = pgm_read_byte(bitmap + 1);
	bitmap = bitmap + 2; //add an offset to the pointer to start after the width and height
#if (ENABLE_BITMAPS > 0)
	int8_t i, j, byteWidth = (w + 7) / 8;
	for (j = 0; j < h; j++) {
	   for (i = 0; i < w; i++) {
	      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (B10000000 >> (i % 8))) {
	         drawPixel(x + i, y + j, color);
	      }
	   }
	}
#else
	drawRect(x, y, w, h);
#endif
}

void Adafruit_GFX::drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap,
	uint8_t rotation, uint8_t flip) {
	if ((rotation == NOROT) && (flip == NOFLIP)) {
		drawBitmap(x, y, bitmap); //use the faster algorithm
		return;
	}
	uint8_t w = pgm_read_byte(bitmap);
	uint8_t h = pgm_read_byte(bitmap + 1);
	bitmap = bitmap + 2; //add an offset to the pointer to start after the width and height
#if (ENABLE_BITMAPS > 0)
	int8_t i, j, //coordinates in the raw bitmap
		k, l, //coordinates in the rotated/flipped bitmap
		byteNum, bitNum, byteWidth = (w + 7) >> 3;

	rotation %= 4;

	for (i = 0; i < w; i++) {
		byteNum = i / 8;
		bitNum = i % 8;
		for (j = 0; j < h; j++) {
			if (pgm_read_byte(bitmap + j * byteWidth + byteNum) & (B10000000 >> bitNum)) {
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
					flip %= 4;
					if (flip & B00000001) { //horizontal flip
						k = w - k - 1;
					}
					if (flip & B00000010) { //vertical flip
						l = h - l;
					}
				}
				k += x; //place the bitmap on the screen
				l += y;
				drawPixel(k, l, color);
			}
		}
	}
#else
	drawRect(x, y, w, h);
#endif
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void Adafruit_GFX::drawXBitmap(int16_t x, int16_t y,
 const uint8_t *bitmap, int16_t w, int16_t h) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) byte >>= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x01) drawPixel(x+i, y+j, color);
    }
  }
}


void Adafruit_GFX::drawImage(int16_t x, int16_t y, Image img) {
	int16_t w1 = img._width; //width of the source image
	int16_t h1 = img._height; //height of the source image
	if ((x > _width) || ((x + w1) < 0) || (y > _height) || ((y + h1) < 0)) return;
	//horizontal cropping
	int16_t i2offset = 0;
	int16_t w2cropped = w1;
	if (x < 0) {
		i2offset = -x;
		w2cropped = w1 + x;
	}
	else {
		if ((x + w1) > _width) {
			w2cropped = _width - x;
		}
	}

	//vertical cropping
	int16_t j2offset = 0;
	int16_t h2cropped = h1;
	if (y < 0) {
		j2offset = -y;
		h2cropped = h1 + y;
	}
	else {
		if ((y + h1) > _height) {
			h2cropped = _height - y;
		}
	}


	if ((img.colorMode == ColorMode::INDEX) && (colorMode == ColorMode::RGB565)) {
		for (int j2 = 0; j2 < h2cropped; j2++) {
			uint16_t destLineArray[w2cropped];
			uint16_t *destLine = destLineArray;
			uint16_t *srcLine;
				
			srcLine = img._buffer + (((j2 + j2offset) * w1) + i2offset) / 4;

			indexTo565(destLine, srcLine, Adafruit_GFX::colorIndex, w2cropped);
			//memset(destLineArray, 0xFF, w2cropped * 2);

			drawBufferedLine(
				x + i2offset,
				y + j2offset + j2,
				destLine,
				w2cropped);
		}
		return;
	}

	for (int j2 = 0; j2 < h2cropped; j2++) { //j2 : offseted vertical coordinate in the destination image
		drawBufferedLine(
			x + i2offset,
			y + j2offset + j2,
			img._buffer + ((j2 + j2offset) * w1) + i2offset,
			w2cropped);
	}

	//the dumb way
	/*for (int i = 0; i < img._width; i++) {
	for (int j = 0; j < img._height; j++) {
	drawPixel(x + i, y + j, img._buffer[i + img._width*j]);
	}
	}*/
}

void Adafruit_GFX::drawImage(int16_t x, int16_t y, Image img, int16_t w2, int16_t h2) {

	if ((x > _width) || ((x + abs(w2)) < 0) || (y > _height) || ((y + abs(h2)) < 0) || (w2 == 0) || (h2 == 0)) return;

	int16_t w = img._width;
	int16_t h = img._height;

	if ((w == w2) && (h == h2)) {
		drawImage(x, y, img);
		return;
	}

	boolean invertX = (w2 < 0);
	boolean invertY = (h2 < 0);
	w2 = abs(w2);
	h2 = abs(h2);
	uint16_t bufferLine[w2];


	//horizontal cropping
	int16_t w2cropped = w2; //width of the cropped buffer
	int16_t i2offset = 0;
	if (x < 0) {
		i2offset = -x;
		w2cropped = w2 + x;
	}
	else {
		if ((x + w2) > _width) {
			w2cropped = _width - x;
		}
	}

	//vertical cropping
	int16_t h2cropped = h2;
	int16_t j2offset = 0;
	if (y < 0) {
		j2offset = -y;
		h2cropped = h2 + y;
	}
	else {
		if ((y + h2) > _height) {
			h2cropped = _height - y;
		}
	}

	for (int16_t j2 = 0; j2 < h2cropped; j2++) { //j2: offseted vertical coordinate in destination
		uint16_t j = h * (j2 + j2offset) / h2; //j: vertical coordinate in source image
		j = invertY ? h - 1 - j : j;
		for (int16_t i2 = 0; i2 < w2cropped; i2++) { //i2: offseted horizontal coordinate in desination
			uint16_t i = w * (i2 + i2offset) / w2; //i: horizontal coordinate in original image
			i = invertX ? w - 1 - i : i;
			bufferLine[i2] = img._buffer[(j * w) + i];
		}
		drawBufferedLine(x + i2offset, y + j2 + j2offset, bufferLine, w2cropped);
	}
}


#if ARDUINO >= 100
size_t Adafruit_GFX::write(uint8_t c) {
#else
void Adafruit_GFX::write(uint8_t c) {
#endif

  if(!gfxFont) { // 'Classic' built-in font

    if(c == '\n') {
      cursorY += textsize*8;
      cursorX  = 0;
    } else if(c == '\r') {
      // skip em
    } else {
      if(wrap && ((cursorX + textsize * 6) >= _width)) { // Heading off edge?
        cursorX  = 0;            // Reset x to zero
        cursorY += textsize * 8; // Advance y one line
      }
      drawChar(cursorX, cursorY, c, color, bgcolor, textsize);
      cursorX += textsize * 6;
    }

  } else { // Custom font

    if(c == '\n') {
      cursorX  = 0;
      cursorY += (int16_t)textsize *
                  (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if(c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        uint8_t   c2    = c - pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c2]);
        uint8_t   w     = pgm_read_byte(&glyph->width),
                  h     = pgm_read_byte(&glyph->height);
        if((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
          if(wrap && ((cursorX + textsize * (xo + w)) >= _width)) {
            // Drawing character would go off right edge; wrap to new line
            cursorX  = 0;
            cursorY += (int16_t)textsize *
                        (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
          }
          drawChar(cursorX, cursorY, c, color, bgcolor, textsize);
        }
        cursorX += pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
      }
    }

  }
#if ARDUINO >= 100
  return 1;
#endif
}

// Draw a characteri
void Adafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c,
 uint16_t color, uint16_t bg, uint8_t size) {

  if(!gfxFont) { // 'Classic' built-in font

    if((x >= _width)            || // Clip right
       (y >= _height)           || // Clip bottom
       ((x + 6 * size - 1) < 0) || // Clip left
       ((y + 8 * size - 1) < 0))   // Clip top
      return;

    if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

    for(int8_t i=0; i<6; i++ ) {
      uint8_t line;
      if(i < 5) line = pgm_read_byte(font+(c*5)+i);
      else      line = 0x0;
      for(int8_t j=0; j<8; j++, line >>= 1) {
        if(line & 0x1) {
          if(size == 1) drawPixel(x+i, y+j, color);
          else          fillRect(x+(i*size), y+(j*size), size, size);
        } else if(bg != color) {
          if(size == 1) drawPixel(x+i, y+j, bg);
          else          fillRect(x+i*size, y+j*size, size, size);
        }
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
             h  = pgm_read_byte(&glyph->height),
             xa = pgm_read_byte(&glyph->xAdvance);
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
            drawPixel(x+xo+xx, y+yo+yy, color);
          } else {
            fillRect(x+(xo16+xx)*size, y+(yo16+yy)*size, size, size);
          }
        }
        bits <<= 1;
      }
    }

  } // End classic vs custom font
}

void Adafruit_GFX::setCursor(int16_t x, int16_t y) {
  cursorX = x;
  cursorY = y;
}

int16_t Adafruit_GFX::getCursorX(void) const {
  return cursorX;
}

int16_t Adafruit_GFX::getCursorY(void) const {
  return cursorY;
}

void Adafruit_GFX::setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}

void Adafruit_GFX::setColor(uint16_t c) {
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  color = bgcolor = c;
}

void Adafruit_GFX::setColor(uint16_t c, uint16_t b) {
  color   = c;
  bgcolor = b;
}

void Adafruit_GFX::setTextWrap(boolean w) {
  wrap = w;
}

uint8_t Adafruit_GFX::getRotation(void) const {
  return rotation;
}

void Adafruit_GFX::setRotation(uint8_t x) {
  rotation = (x & 3);
  switch(rotation) {
   case 0:
   case 2:
    _width  = WIDTH;
    _height = HEIGHT;
    break;
   case 1:
   case 3:
    _width  = HEIGHT;
    _height = WIDTH;
    break;
  }
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void Adafruit_GFX::cp437(boolean x) {
  _cp437 = x;
}

void Adafruit_GFX::setFont(const GFXfont *f) {
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

// Pass string and a cursor position, returns UL corner and W,H.
void Adafruit_GFX::getTextBounds(char *str, int16_t x, int16_t y,
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
              gx1, gy1, gx2, gy2, ts = (int16_t)textsize,
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
            if(wrap && ((x + (((int16_t)xo + gw) * ts)) >= _width)) {
              // Line wrap
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
          if(wrap && ((x + textsize * 6) >= _width)) {
            x  = 0;            // Reset x to 0
            y += textsize * 8; // Advance y by 1 line
            if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
            lineWidth  = textsize * 6; // First char on new line
          } else { // No line wrap, just keep incrementing X
            lineWidth += textsize * 6; // Includes interchar x gap
          }
        } // Carriage return = do nothing
      } else { // Newline
        x  = 0;            // Reset x to 0
        y += textsize * 8; // Advance y by 1 line
        if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
        lineWidth = 0;     // Reset lineWidth for new line
      }
    }
    // End of string
    if(lineWidth) y += textsize * 8; // Add height of last (or only) line
    if(lineWidth > maxWidth) maxWidth = lineWidth; // Is the last or only line the widest?
    *w = maxWidth - 1;               // Don't include last interchar x gap
    *h = y - *y1;

  } // End classic vs custom font
}

// Same as above, but for PROGMEM strings
void Adafruit_GFX::getTextBounds(const __FlashStringHelper *str,
 int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
  uint8_t *s = (uint8_t *)str, c;

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
              gx1, gy1, gx2, gy2, ts = (int16_t)textsize,
              ya = ts * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);

    while((c = pgm_read_byte(s++))) {
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
            if(wrap && ((x + (((int16_t)xo + gw) * ts)) >= _width)) {
              // Line wrap
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

    while((c = pgm_read_byte(s++))) {
      if(c != '\n') { // Not a newline
        if(c != '\r') { // Not a carriage return, is normal char
          if(wrap && ((x + textsize * 6) >= _width)) {
            x  = 0;            // Reset x to 0
            y += textsize * 8; // Advance y by 1 line
            if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
            lineWidth  = textsize * 6; // First char on new line
          } else { // No line wrap, just keep incrementing X
            lineWidth += textsize * 6; // Includes interchar x gap
          }
        } // Carriage return = do nothing
      } else { // Newline
        x  = 0;            // Reset x to 0
        y += textsize * 8; // Advance y by 1 line
        if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
        lineWidth = 0;     // Reset lineWidth for new line
      }
    }
    // End of string
    if(lineWidth) y += textsize * 8; // Add height of last (or only) line
    if(lineWidth > maxWidth) maxWidth = lineWidth; // Is the last or only line the widest?
    *w = maxWidth - 1;               // Don't include last interchar x gap
    *h = y - *y1;

  } // End classic vs custom font
}

// Return the size of the display (per current rotation)
int16_t Adafruit_GFX::width(void) const {
  return _width;
}

int16_t Adafruit_GFX::height(void) const {
  return _height;
}

void Adafruit_GFX::invertDisplay(boolean i) {
  // Do nothing, must be subclassed if supported by hardware
}