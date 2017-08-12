// Image.h

#ifndef _GAMEBUINO_META_IMAGE_h
#define _GAMEBUINO_META_IMAGE_h

#include <Arduino.h>
#include "Graphics.h"

namespace Gamebuino_Meta {

class Frame_Handler {
public:
	Frame_Handler(Image* _img);
	~Frame_Handler();
	virtual void next() = 0;
	virtual void set(uint16_t frame) = 0;
	virtual void first();
	virtual uint32_t getBufferSizeWithFrames();
	void allocateBuffer();
protected:
	Image* img;
	uint16_t* buf;
	uint16_t bufferSize;
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
	uint32_t getBufferSizeWithFrames();
};

#define DEFAULT_FRAME_LOOP 1

class Image : public Graphics {
public:
	Image();
	~Image();
	Image(const Image&);

	// ram constructors
	Image(uint16_t w, uint16_t h, ColorMode col, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(uint16_t w, uint16_t h, ColorMode col, uint8_t fl = DEFAULT_FRAME_LOOP);
	Image(uint16_t w, uint16_t h, uint16_t frames = 1, ColorMode col = ColorMode::rgb565, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(uint16_t w, uint16_t h, uint16_t frames = 1, ColorMode col = ColorMode::rgb565, uint8_t fl = DEFAULT_FRAME_LOOP);

	// flash constructors
	Image(const uint16_t* buffer, ColorMode col, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(const uint16_t* buffer, ColorMode col, uint8_t fl = DEFAULT_FRAME_LOOP);
	Image(const uint16_t* buffer, uint16_t frames = 1, ColorMode col = ColorMode::rgb565, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(const uint16_t* buffer, uint16_t frames = 1, ColorMode col = ColorMode::rgb565, uint8_t fl = DEFAULT_FRAME_LOOP);
	
	// flash indexed constructors
	Image(const uint8_t* buffer, ColorMode col, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(const uint8_t* buffer, ColorMode col, uint8_t fl = DEFAULT_FRAME_LOOP);
	Image(const uint8_t* buffer, uint16_t frames = 1, ColorMode col = ColorMode::index, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(const uint8_t* buffer, uint16_t frames = 1, ColorMode col = ColorMode::index, uint8_t fl = DEFAULT_FRAME_LOOP);

	// SD constructors
	Image(char* filename, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(char* filename, uint8_t fl = DEFAULT_FRAME_LOOP);
	Image(uint16_t w, uint16_t h, char* filename, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(uint16_t w, uint16_t h, char* filename, uint8_t fl = DEFAULT_FRAME_LOOP);
	
	
	void nextFrame();
	void setFrame(uint16_t frame);
	void freeBuffer();
	void drawPixel(int16_t x, int16_t y);
	void fillScreen();
	void fillScreen(Color color);
	void fillScreen(ColorIndex color);
	void drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, Image& img);
	void allocateBuffer();
	
	bool startRecording(char* filename);
	void stopRecording(bool output = false);
	bool save(char* filename);

	uint16_t getBufferSize();

	//`frame_looping` holds every how many frames it should get updated
	//`frame_loopcounter` is the counter for that.
	//Typically `frame_looping` is 1 if you want to update every frame.
	//You set that manually to zero in your thing meaning that you'd have to manually swap frames
    //`frames` is the total number of frames and `frame` is the current one
	//`frame_handler` is an internal object thing, there are three different kinds of frame handlers: ram buffer, flash buffer and sd card
	//`last_frame` is used to checking that, if you draw the image multiple times within a frame, it'll only advance the animation on the first one
	
	uint16_t *_buffer;
	uint16_t frames;
	uint16_t frame = 0;
	Frame_Handler* frame_handler;
	uint8_t frame_looping;
	union {
		struct {
			uint8_t last_frame;
			uint8_t frame_loopcounter;
		};
		uint16_t bufferSize;
	};
private:
	bool isObjectCopy;
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_IMAGE_h
