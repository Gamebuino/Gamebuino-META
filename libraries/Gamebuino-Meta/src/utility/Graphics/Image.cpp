// 
// 
// 

#include "Image.h"
#include "../Graphics-SD.h"
#include "../../Gamebuino-Meta.h"

namespace Gamebuino_Meta {

Frame_Handler::Frame_Handler(Image* _img) {
	img = _img;
	buf = img->_buffer;
}

void Frame_Handler::first() {
	set(0);
}

uint32_t Frame_Handler::getBufferSizeWithFrames() {
	return img->getBufferSize();
}

void Frame_Handler::deallocateBuffer() {
	if (!buf || (uint32_t)buf < 0x20000000) {
		return;
	}
	free(buf);
	buf = 0;
}

void Frame_Handler::allocateBuffer() {
	deallocateBuffer();
	uint32_t bytes = getBufferSizeWithFrames();
	if ((buf = (uint16_t *)malloc(bytes))) {
		memset(buf, 0, bytes);
	}
	img->_buffer = buf;
}

Frame_Handler_Mem::Frame_Handler_Mem(Image* _img) : Frame_Handler(_img) {
	
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
	deallocateBuffer();
}

uint32_t Frame_Handler_RAM::getBufferSizeWithFrames() {
	return img->getBufferSize() * img->frames;
}


/********
 * start of actual image class
 ********/

Image::Image() : Graphics(0, 0){
}

// ram constructors
Image::Image(uint16_t w, uint16_t h, ColorMode col, uint8_t fl) : Graphics(w, h) {
	init(w, h, col, fl);
}
void Image::init(uint16_t w, uint16_t h, ColorMode col, uint8_t fl) {
	init(w, h, 1, col, fl);
}

Image::Image(uint16_t w, uint16_t h, uint16_t _frames, ColorMode col, uint8_t fl) : Graphics(0, 0) {
	init(w, h, frames, col, fl);
}
void Image::init(uint16_t w, uint16_t h, uint16_t _frames, ColorMode col, uint8_t fl) {
	if (frame_handler) {
		delete frame_handler;
	}
	transparentColor = 0xF81F;
	frames = _frames;
	colorMode = col;
	_width = w;
	_height = h;
	frame_looping = fl;
	frame_loopcounter = 0;
	frame_handler = new Frame_Handler_RAM(this);
	setFrame(0);
}

// flash constructors
Image::Image(const uint16_t* buffer, ColorMode col, uint8_t fl) : Graphics(0, 0) {
	init(buffer, col, fl);
}
void Image::init(const uint16_t* buffer, ColorMode col, uint8_t fl) {
	init(buffer, 1, col, fl);
}

Image::Image(const uint16_t* buffer, uint16_t frames, ColorMode col, uint8_t fl) : Graphics(0, 0) {
	init(buffer, frames, col, fl);
}
void Image::init(const uint16_t* buffer, uint16_t _frames, ColorMode col, uint8_t fl) {
	if (frame_handler) {
		delete frame_handler;
	}
	transparentColor = 0xF81F;
	frames = _frames;
	colorMode = col;
	uint16_t* buf = (uint16_t*)buffer;
	_width = *(buf++);
	_height = *(buf++);
	_buffer = buf;
	frame_looping = fl;
	frame_loopcounter = 0;
	frame_handler = new Frame_Handler_Mem(this);
	setFrame(0);
}

// SD constructors
Image::Image(char* filename, uint8_t fl) : Graphics(0, 0) {
	init(filename, fl);
}
void Image::init(char* filename, uint8_t fl) {
	init(0, 0, filename, fl);
}

Image::Image(uint16_t w, uint16_t h, char* filename, uint8_t fl) : Graphics(w, h) {
	init(w, h, filename, fl);
}
void Image::init(uint16_t w, uint16_t h, char* filename, uint8_t fl) {
	if (frame_handler) {
		delete frame_handler;
	}
	transparentColor = 0;
	_width = w;
	_height = h;
	last_frame = (gb.frameCount & 0xFF) - 1;
	frame_looping = fl;
	frame_loopcounter = 0;
	frame = 0;
	frame_handler = new Frame_Handler_SD(this);
	// for the SD handler we do NOT set frame to zero
	// unlike the other handlers the SD handler must be lazy-inited
	// and calling setFrame(0) here would trigger the lazy init already
	//setFrame(0);
	((Frame_Handler_SD*)frame_handler)->init(filename);
}


Image::~Image() {
	delete frame_handler;
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
	return Graphics_SD::startRecording(this, filename);
}

void Image::stopRecording(bool output) {
	Graphics_SD::stopRecording(this, output);
}

bool Image::save(char* filename) {
	return Graphics_SD::save(this, filename);
}

void Image::allocateBuffer() {
	frame_handler->allocateBuffer();
}

void Image::nextFrame() {
	if (frames) {
		if (frames == 1 || !frame_looping || last_frame == gb.frameCount & 0xFF) {
			return;
		}
	}
	last_frame = gb.frameCount & 0xFF;
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
	last_frame = gb.frameCount & 0xFF; // we already loaded this frame!
	frame_loopcounter = 0;
}

void Image::drawPixel(int16_t x, int16_t y) {
	if (!_buffer) {
		return;
	}
	if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
		return;
	}
	if (colorMode == ColorMode::rgb565) {
		_buffer[x + y * _width] = (uint16_t)color;
		return;
	}
	if (colorMode == ColorMode::index) {
		uint16_t addr = ((_width + 1) / 2) * y + x / 2;
		uint8_t* buf = (uint8_t*)_buffer;
		if (!(x % 2)) { //odd pixels
			buf[addr] &= 0x0F; //clear
			buf[addr] |= ((uint8_t)color << 4); //set
		} else { //even pixels
			buf[addr] &= 0xF0; //clear
			buf[addr] |= ((uint8_t)color & 0x0F); //set
		}
		return;
	}
}

void Image::fillScreen() {
	if (_buffer) {
		if (colorMode == ColorMode::rgb565) {
			uint8_t hi = (uint16_t)color >> 8, lo = (uint16_t)color & 0xFF;
			if (hi == lo) {
				memset(_buffer, lo, WIDTH * HEIGHT * 2);
			}
			else {
				uint16_t i, pixels = WIDTH * HEIGHT;
				for (i = 0; i<pixels; i++) _buffer[i] = (uint16_t)color;
			}
		}
		
		if (colorMode == ColorMode::index) {
			uint8_t pack = ((uint8_t)color) + ((uint8_t)color << 4);
			memset(_buffer, pack, WIDTH * HEIGHT / 2);
		}
	}
}

void Image::drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, Image& img) {
	if (colorMode == ColorMode::index) {
		// TODO: transparent index color
		uint8_t *src = (uint8_t*)buffer;
		uint8_t *dst = (uint8_t*)_buffer + y*((_width + 1) / 2) + x/2;
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
		return;
	}
	if ((alpha == 255) && (tint == 0xFFFF)) { //no alpha blending and not tinting
		if (!img.transparentColor) { //no transparent color set
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
				int16_t r2 = color2 & B11111;
				int16_t g2 = (color2 >> 5) & B111111;
				int16_t b2 = (color2 >> 11) & B11111;
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
			int16_t r2 = color2 & B11111;
			int16_t g2 = (color2 >> 5) & B111111;
			int16_t b2 = (color2 >> 11) & B11111;
			r1[i] = min((r1[i] * alpha + r2 * 255) / 256, 31);
			g1[i] = min((g1[i] * alpha + g2 * 255) / 256, 63);
			b1[i] = min((b1[i] * alpha + b2 * 255) / 256, 31);
		}
		break;
	case BlendMode::subtract:
		for (uint8_t i = 0; i < w; i++) {
			if (img.transparentColor && buffer[i] == img.transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & B11111;
			int16_t g2 = (color2 >> 5) & B111111;
			int16_t b2 = (color2 >> 11) & B11111;
			r1[i] = max((r2 * 255 - r1[i] * alpha) / 256, 0);
			g1[i] = max((g2 * 255 - g1[i] * alpha) / 256, 0);
			b1[i] = max((b2 * 255 - b1[i] * alpha) / 256, 0);
		}
		break;
	case BlendMode::multiply:
		for (uint8_t i = 0; i < w; i++) {
			if (img.transparentColor && buffer[i] == img.transparentColor) continue;
			uint16_t color2 = thisLine[i];
			int16_t r2 = color2 & B11111;
			int16_t g2 = (color2 >> 5) & B111111;
			int16_t b2 = (color2 >> 11) & B11111;
			r1[i] = max((r2 * r1[i]) / 32, 0);
			g1[i] = max((g2 * g1[i]) / 64, 0);
			b1[i] = max((b2 * b1[i]) / 32, 0);
		}
		break;
	case BlendMode::screen:
		for (uint8_t i = 0; i < w; i++) {
			if (img.transparentColor && buffer[i] == img.transparentColor) continue;
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

	for (uint8_t i = 0; i < w; i++) {
		if (img.transparentColor && buffer[i] == img.transparentColor) continue;
		thisLine[i] = (b1[i] << 11) + (g1[i] << 5) + r1[i];
	}
}

} // namespace Gamebuino_Meta
