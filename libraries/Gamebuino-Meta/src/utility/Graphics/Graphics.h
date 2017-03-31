#ifndef _GAMEBUINO_META_GRAPHICS_H_
#define _GAMEBUINO_META_GRAPHICS_H_

#if ARDUINO >= 100
#include "Arduino.h"
#include "Print.h"
#else
#include "WProgram.h"
#endif

#include "gfxfont.h"

namespace Gamebuino_Meta {


enum class Color : uint16_t {
	white		= 0xFFFF,
	gray		= 0xACD0,
	darkgray	= 0x72C7,
	black		= 0x0000,
	purple		= 0x8A39,
	pink		= 0xCA30,
	red			= 0xD8E4,
	orange		= 0xFD42,
	brown		= 0xCC68,
	beige		= 0xFEB2,
	yellow		= 0xF720,
	lightgreen	= 0x8668,
	green		= 0x044A,
	darkblue	= 0x0210,
	blue		= 0x4439,
	lightblue	= 0x7DDF,
};

enum class ColorIndex : uint8_t {
	black,
	darkblue,
	purple,
	green,
	brown,
	darkgray,
	gray,
	white,
	red,
	orange,
	yellow,
	lightgreen,
	lightblue,
	blue,
	pink,
	beige,
};

#define ENABLE_BITMAPS 1
//for extended bitmap function :
#define NOROT 0
#define ROTCCW 1
#define ROT180 2
#define ROTCW 3
#define NOFLIP 0
#define FLIPH 1
#define FLIPV 2
#define FLIPVH 3

class Image;

enum class BlendMode : uint8_t {
	blend,
	add,
	subtract,
	multiply,
	screen,
};

enum class ColorMode : uint8_t {
	rgb565,
	index,
};

class Graphics : public Print {

public:

	Graphics(int16_t w, int16_t h); // Constructor

	// This MUST be defined by the subclass:
	virtual void drawPixel(int16_t x, int16_t y) = 0;
	virtual void drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w) = 0;


	// These MAY be overridden by the subclass to provide device-specific
	// optimized code.  Otherwise 'generic' versions are used.
	virtual void
		drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1),
		drawFastVLine(int16_t x, int16_t y, int16_t h),
		drawFastHLine(int16_t x, int16_t y, int16_t w),
		drawRect(int16_t x, int16_t y, int16_t w, int16_t h),
		fillRect(int16_t x, int16_t y, int16_t w, int16_t h),
		fillScreen(),
		fillScreen(Color color), //legacy version
		invertDisplay(boolean i),
		drawImage(int16_t x, int16_t y, Image img),
		drawImage(int16_t x, int16_t y, Image img, int16_t w2, int16_t h2);

	// These exist only with Graphics (no subclass overrides)
	void
		drawCircle(int16_t x0, int16_t y0, int16_t r),
		drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername),
		fillCircle(int16_t x0, int16_t y0, int16_t r),
		fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
			int16_t delta),
		drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
			int16_t x2, int16_t y2),
		fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
			int16_t x2, int16_t y2),
		drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
			int16_t radius),
		fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
			int16_t radius),
		//Adafruit bitmaps
		/*drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
			int16_t w, int16_t h),
		drawBitmap(int16_t x, int16_t y, uint8_t *bitmap,
			int16_t w, int16_t h),
		drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
			int16_t w, int16_t h),*/

		//Gamebuino legacy bitmaps
		drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap),
		drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap, uint8_t rotation, uint8_t flip),

		drawChar(int16_t x, int16_t y, unsigned char c, uint8_t size),
		setCursor(int16_t x, int16_t y),
		setColor(Color c),
		setColor(Color c, Color bg),
		setColor(ColorIndex c),
		setColor(ColorIndex c, ColorIndex bg),
		setColor(uint8_t c),
		setColor(uint8_t c, uint8_t bg),
		setFontSize(uint8_t s),
		setTextWrap(boolean w),
		setRotation(uint8_t r),
		cp437(boolean x=true),
		setFont(const GFXfont *f = NULL), //adafruit custom font
		setFont(const uint8_t* f), //gamebuino legacy font
		getTextBounds(char *string, int16_t x, int16_t y,
			int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h),
		getTextBounds(const __FlashStringHelper *s, int16_t x, int16_t y,
			int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

	//Gamebuino legacy function
	boolean getBitmapPixel(const uint8_t* bitmap, uint8_t x, uint8_t y);


	static void indexTo565(uint16_t *dest, uint8_t *src, Color *index, uint16_t length, bool skipFirst);

	static uint16_t transparentColor;
	static uint8_t alpha;
	static uint16_t tint;
	static BlendMode blendMode;
	static Color *colorIndex;
	ColorMode colorMode;

#if ARDUINO >= 100
	virtual size_t write(uint8_t);
#else
	virtual void   write(uint8_t);
#endif

	int16_t height(void) const;
	int16_t width(void) const;

	uint8_t getRotation(void) const;

	// get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
	int16_t getCursorX(void) const;
	int16_t getCursorY(void) const;

 //protected:
	const int16_t
		WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
	int16_t
		_width, _height, // Display w/h as modified by current rotation
		cursorX, cursorY;
	static Color
		color, bgcolor;
	uint8_t
		fontSize,
		rotation;
	boolean
		textWrap,   // If set, 'wrap' text at right edge of display
		_cp437; // If set, use correct CP437 charset (default is off)
	GFXfont *gfxFont; //adafruit custom font

	uint8_t *font; //gamebuino legacy font
	uint8_t fontWidth, fontHeight; //gamebuino legacy font size
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_GRAPHICS_H_
