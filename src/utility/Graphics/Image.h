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

#ifndef _GAMEBUINO_META_IMAGE_h
#define _GAMEBUINO_META_IMAGE_h

#include "../../config/config.h"
#include "Graphics.h"

namespace Gamebuino_Meta {

class Frame_Handler {
public:
	Frame_Handler(Image* _img);
	virtual ~Frame_Handler();
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
	virtual ~Frame_Handler_Mem();
	void next();
	void set(uint16_t frame);
};

class Frame_Handler_RAM : public Frame_Handler_Mem {
public:
	Frame_Handler_RAM(Image* _img);
	~Frame_Handler_RAM();
	uint32_t getBufferSizeWithFrames();
};

#define DEFAULT_FRAME_LOOP 1

class Image : public Graphics {
public:
	using Graphics::drawImage;
	using Graphics::drawBitmap;
	Image();
	~Image();
	Image(const Image&);

	// ram constructors
	Image(uint16_t w, uint16_t h, uint16_t frames, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(uint16_t w, uint16_t h, uint16_t frames, uint8_t fl = DEFAULT_FRAME_LOOP);
	Image(uint16_t w, uint16_t h, ColorMode col = ColorMode::rgb565, uint16_t frames = 1, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(uint16_t w, uint16_t h, ColorMode col = ColorMode::rgb565, uint16_t frames = 1, uint8_t fl = DEFAULT_FRAME_LOOP);

	// flash constructors
	Image(const uint16_t* buffer);
	void init(const uint16_t* buffer);
	
	// flash indexed constructors
	Image(const uint8_t* buffer);
	void init(const uint8_t* buffer);

	// SD constructors
	Image(char* filename, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(char* filename, uint8_t fl = DEFAULT_FRAME_LOOP);
	Image(uint16_t w, uint16_t h, char* filename, uint8_t fl = DEFAULT_FRAME_LOOP);
	void init(uint16_t w, uint16_t h, char* filename, uint8_t fl = DEFAULT_FRAME_LOOP);
	
	void drawFastHLine(int16_t x, int16_t y, int16_t w) override __attribute__((optimize("-O3")));

	
	void nextFrame();
	void setFrame(uint16_t frame);
	void _drawPixel(int16_t x, int16_t y);
	void _fill() override;
	void drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, Image& img);
	void allocateBuffer();
	
	uint16_t getPixel(int16_t x, int16_t y);
	Color getPixelColor(int16_t x, int16_t y);
	ColorIndex getPixelIndex(int16_t x, int16_t y);
	
	bool startRecording(char* filename);
	void stopRecording(bool output = false);
	bool save(char* filename);

	uint16_t getBufferSize();
	
	void drawChar(int16_t x, int16_t y, unsigned char c, uint8_t size);
	
	void drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap);
	void drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap, uint8_t rotation, uint8_t flip);
	
	void drawImage(int16_t x, int16_t y, Image& img);
	
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
	void freshStart();
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_IMAGE_h
