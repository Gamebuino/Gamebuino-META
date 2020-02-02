#ifndef _GAMEBUINO_META_GRAPHICS_H_
#define _GAMEBUINO_META_GRAPHICS_H_

#include "../../config/config.h"

#include "gfxfont.h"

#include "../Language.h"

namespace Gamebuino_Meta {


enum class Color : uint16_t {
	white		= 0xFFFF,
	gray		= 0xACD0,
	darkgray	= 0x5268,
	black		= 0x0000,
	purple		= 0x9008,
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
	rgb565 = 0,
	index,
};

class Graphics : public Print_Language {

public:
	using Print_Language::print;
	using Print_Language::println;
#if USE_PRINTF
	using Print_Language::printf;
#endif // USE_PRINTF
	using Print_Language::write;
	Graphics(int16_t w, int16_t h); // Constructor
	virtual ~Graphics();

	// This MUST be defined by the subclass:
	virtual void _drawPixel(int16_t x, int16_t y) = 0;
	virtual void drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, Image& img) = 0;


	// These MAY be overridden by the subclass to provide device-specific
	// optimized code.  Otherwise 'generic' versions are used.
	virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
	virtual void drawFastVLine(int16_t x, int16_t y, int16_t h);
	virtual void drawFastHLine(int16_t x, int16_t y, int16_t w);
	virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h);
	virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h);
	virtual void _fill();
	virtual void invertDisplay(bool i);
	virtual void drawImage(int16_t x, int16_t y, Image& img);
	virtual void drawImage(int16_t x, int16_t y, Image& img, int16_t w2, int16_t h2);
	virtual void drawImage(int16_t x, int16_t y, Image& img, int16_t x2, int16_t y2, int16_t w2, int16_t h2);
	template <typename T>
	void drawImage(int16_t x, int16_t y, T img) {
		Image i(img);
		drawImage(x, y, i);
	};
	template <typename T>
	void drawImage(int16_t x, int16_t y, T img, int16_t w2, int16_t h2) {
		Image i(img);
		drawImage(x, y, i, w2, h2);
	};
	template <typename T>
	void drawImage(int16_t x, int16_t y, T img, int16_t x2, int16_t y2, int16_t w2, int16_t h2) {
		Image i(img);
		drawImage(x, y, i, x2, y2, w2, h2);
	};
	void drawImageCrop(int16_t x, int16_t y, int16_t w1, int16_t i2offset, int16_t w2cropped, int16_t j2offset, int16_t h2cropped, Image& img);
	virtual void drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap);
	virtual void drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap, int8_t scale);
	virtual void drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap, uint8_t rotation, uint8_t flip);

	// These exist only with Graphics (no subclass overrides)
	void drawPixel(int16_t x, int16_t y);
	void drawPixel(int16_t x, int16_t y, Color c);
	void drawPixel(int16_t x, int16_t y, ColorIndex c);
	void fill();
	void fill(Color color);
	void fill(ColorIndex color);
	void clearTextVars();
	void clear();
	void clear(Color bgcolor);
	void clear(ColorIndex bgcolor);
	
	void drawCircle(int16_t x0, int16_t y0, int16_t r);
	void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername);
	void fillCircle(int16_t x0, int16_t y0, int16_t r);
	void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta);
	void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius);
	void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius);

	
	virtual void drawChar(int16_t x, int16_t y, unsigned char c, uint8_t size);
	Color setTmpColor(Color c);
	Color setTmpColor(ColorIndex c);
	void setColor(Color c);
	void setColor(Color c, Color bg);
	void setColor(ColorIndex c);
	void setColor(ColorIndex c, ColorIndex bg);
	void setColor(uint8_t c);
	void setColor(uint8_t c, uint8_t bg);
	void setColor(uint8_t r, uint8_t g, uint8_t b);
	void setTransparentColor(Color c);
	void setTransparentColor(ColorIndex c);
	void clearTransparentColor();
	static void setCursorX(int16_t x);
	static void setCursorY(int16_t y);
	static void setCursor(int16_t x, int16_t y);
	void setFontSize(uint8_t s);
	void setTextWrap(bool w);
	void cp437(bool x=true);
	void setFont(const GFXfont *f = NULL); //adafruit custom font
	void setFont(const uint8_t* f); //gamebuino legacy font
	void getTextBounds(const char *str, uint16_t *w, uint16_t *h);
	void getTextBounds(char *string, int16_t x, int16_t y, 	int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
	void getTextBounds(const char *s, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

	//Gamebuino legacy function
	bool getBitmapPixel(const uint8_t* bitmap, uint8_t x, uint8_t y);


	static void indexTo565(uint16_t *dest, uint8_t *src, Color *index, uint16_t length, bool skipFirst);
	static ColorIndex rgb565ToIndex(Color rgb);
	
	// additional coordinate prints
	
	template <uint8_t N>
	void print(int16_t x, int16_t y, const MultiLang (&l) [N]) {
		setCursor(x, y);
		print(l, N);
	};
	template <typename T>
	void print(int16_t x, int16_t y, T s) {
		setCursor(x, y);
		print(s);
	};
	
	template <uint8_t N>
	void println(int16_t x, int16_t y, const MultiLang (&l) [N]) {
		setCursor(x, y);
		print(l, N);
	};
	template <typename T>
	void println(int16_t x, int16_t y, T s) {
		setCursor(x, y);
		print(s);
	};
	
#if USE_PRINTF
	template <uint8_t N>
	void printf(int16_t x, int16_t y, const MultiLang (&l) [N], ...) {
		setCursor(x, y);
		char buf[PRINTF_BUF];
		va_list ap;
		const char* format = Language::get(l, N);
		va_start(ap, format);
		vsnprintf(buf, sizeof(buf), format, ap);
		write(buf);
		va_end(ap);
	};
	void printf(int16_t x, int16_t y, const char format[], ...) {
		setCursor(x, y);
		char buf[PRINTF_BUF];
		va_list ap;
		va_start(ap, format);
		vsnprintf(buf, sizeof(buf), format, ap);
		write(buf);
		va_end(ap);
	};
#endif // USE_PRINTF

	union {
		uint16_t transparentColor;
		struct {
			uint8_t transparentColorIndex;
			bool useTransparentIndex;
		};
	};
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

	// get current cursor position
	int16_t getCursorX(void) const;
	int16_t getCursorY(void) const;
	uint8_t getFontWidth(void) const;
	uint8_t getFontHeight(void) const;
	
	void setPalette(Color* p);
	void setPalette(const Color* p);
	Color* getPalette();

 //protected:
	int16_t
		_width, _height; // Display w/h
	static int16_t cursorX, cursorY;
	static union ColorUnion {
		Color c;
		struct {
			uint8_t iu;
			uint8_t i;
		};
	} color;
	static union BgcolorUnion {
		Color c;
		struct {
			uint8_t iu;
			uint8_t i;
		};
	} bgcolor;
	static uint8_t
		fontSize;
	static bool
		textWrap,   // If set, 'wrap' text at right edge of display
		_cp437; // If set, use correct CP437 charset (default is off)
	static GFXfont* gfxFont; //adafruit custom font

	static uint8_t* font; //gamebuino legacy font
	static uint8_t fontWidth, fontHeight; //gamebuino legacy font size
};

// make the default index available to programs
extern const Color defaultColorPalette[];

} // namespace Gamebuino_Meta

using Gamebuino_Meta::Image;
using Gamebuino_Meta::BlendMode;
using Gamebuino_Meta::ColorMode;
using Gamebuino_Meta::Color;
using Gamebuino_Meta::ColorIndex;

const Color WHITE = Color::white;
const Color GRAY = Color::gray;
const Color DARKGRAY = Color::darkgray;
const Color BLACK = Color::black;
const Color PURPLE = Color::purple;
const Color PINK = Color::pink;
const Color RED = Color::red;
const Color ORANGE = Color::orange;
const Color BROWN = Color::brown;
const Color BEIGE = Color::beige;
const Color YELLOW = Color::yellow;
const Color LIGHTGREEN = Color::lightgreen;
const Color GREEN = Color::green;
const Color DARKBLUE = Color::darkblue;
const Color BLUE = Color::blue;
const Color LIGHTBLUE = Color::lightblue;

const ColorIndex INDEX_WHITE = ColorIndex::white;
const ColorIndex INDEX_GRAY = ColorIndex::gray;
const ColorIndex INDEX_DARKGRAY = ColorIndex::darkgray;
const ColorIndex INDEX_BLACK = ColorIndex::black;
const ColorIndex INDEX_PURPLE = ColorIndex::purple;
const ColorIndex INDEX_PINK = ColorIndex::pink;
const ColorIndex INDEX_RED = ColorIndex::red;
const ColorIndex INDEX_ORANGE = ColorIndex::orange;
const ColorIndex INDEX_BROWN = ColorIndex::brown;
const ColorIndex INDEX_BEIGE = ColorIndex::beige;
const ColorIndex INDEX_YELLOW = ColorIndex::yellow;
const ColorIndex INDEX_LIGHTGREEN = ColorIndex::lightgreen;
const ColorIndex INDEX_GREEN = ColorIndex::green;
const ColorIndex INDEX_DARKBLUE = ColorIndex::darkblue;
const ColorIndex INDEX_BLUE = ColorIndex::blue;
const ColorIndex INDEX_LIGHTBLUE = ColorIndex::lightblue;

#endif // _GAMEBUINO_META_GRAPHICS_H_
