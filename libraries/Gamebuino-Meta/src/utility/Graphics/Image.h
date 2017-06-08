// Image.h

#ifndef _GAMEBUINO_META_IMAGE_h
#define _GAMEBUINO_META_IMAGE_h

#include <Arduino.h>
#include "Graphics.h"

namespace Gamebuino_Meta {

class Frame_Handler {
public:
	Frame_Handler(Image* _img);
	virtual void next() = 0;
	virtual void set(uint16_t frame) = 0;
	virtual void first();
	virtual uint32_t getBufferSizeWithFrames();
	void allocateBuffer();
protected:
	void deallocateBuffer();
	Image* img;
	uint16_t* buf;
};

class Frame_Handler_Mem : public Frame_Handler {
public:
	Frame_Handler_Mem(Image* _img);
	void next();
	void set(uint16_t frame);
};

class Frame_Handler_RAM : public Frame_Handler_Mem {
public:
	Frame_Handler_RAM(Image* _img);
	~Frame_Handler_RAM();
	uint32_t getBufferSizeWithFrames();
};

class Image : public Graphics {
public:
	Image();
	~Image();

	// ram constructors
	Image(uint16_t w, uint16_t h);
	void init(uint16_t w, uint16_t h);
	Image(uint16_t w, uint16_t h, ColorMode col);
	void init(uint16_t w, uint16_t h, ColorMode col);
	Image(uint16_t w, uint16_t h, uint16_t frames);
	void init(uint16_t w, uint16_t h, uint16_t frames);
	Image(uint16_t w, uint16_t h, uint16_t frames, ColorMode col);
	void init(uint16_t w, uint16_t h, uint16_t frames, ColorMode col);

	// flash constructors
	Image(const uint16_t* buffer);
	void init(const uint16_t* buffer);
	Image(const uint16_t* buffer, ColorMode col);
	void init(const uint16_t* buffer, ColorMode col);
	Image(const uint16_t* buffer, uint16_t frames);
	void init(const uint16_t* buffer, uint16_t frames);
	Image(const uint16_t* buffer, uint16_t frames, ColorMode col);
	void init(const uint16_t* buffer, uint16_t frames, ColorMode col);

	// SD constructors
	Image(char* filename);
	void init(char* filename);
	Image(uint16_t w, uint16_t h, char* filename);
	void init(uint16_t w, uint16_t h, char* filename);
	void nextFrame();
	void setFrame(uint16_t frame);
	void freeBuffer(),
		drawPixel(int16_t x, int16_t y),
		fillScreen(Color color),
		drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, Image& img),
		allocateBuffer();

	uint16_t getBufferSize();

	uint16_t *_buffer;
	uint16_t frames;
	uint16_t frame = 0;
	Frame_Handler* frame_handler;
private:
	uint8_t last_frame;
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_IMAGE_h
