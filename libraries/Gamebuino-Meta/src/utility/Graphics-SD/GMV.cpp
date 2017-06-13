#include "GMV.h"
#include "../Misc.h"

#define CONERT_MASK 0xFF000000
#define CONVERT_MAGIC 0xA1000000

namespace Gamebuino_Meta {

GMV::GMV() {
	valid = false;
}

GMV::GMV(Image* _img) {
	img = _img;
}

GMV::GMV(Image* _img, char* filename) {
	valid = false;
	img = _img;
	img->frames = 1;
	file = SD.open(filename, FILE_WRITE);
	if (!file) {
		// couldn't find file, perhaps SD isn't inited yet, falling back to lazy init...
		img->frames = 0;
		return;
	}
	file.rewind();
	uint16_t header = f_read16(&file);
	if (header == 0x4D42) {
		// we got a BMP image!
		BMP bmp = BMP(&file, img);
		if (!bmp.isValid()) {
			return;
		}
		uint16_t name_len = strlen(filename);
		char _filename[name_len + 1];
		strcpy(_filename, filename);
		
		_filename[name_len - 4] = '.';
		_filename[name_len - 3] = 'G';
		_filename[name_len - 2] = 'M';
		_filename[name_len - 1] = 'V';
		_filename[name_len] = '\0';
		
		uint32_t creatorBits = bmp.getCreatorBits();
		if ((creatorBits & CONERT_MASK) != CONVERT_MAGIC || !SD.exists(_filename)) {
			// we still need to convert...
			convertFromBMP(bmp, _filename);
		}
		file.close();
		file = SD.open(_filename, FILE_WRITE);
		if (!file) {
			return;
		}
		file.rewind();
		header = f_read16(&file);
	}
	if (header != 0x5647) { // header "GV"
		// invalid header!
		return;
	}
	header_size = f_read16(&file);
	file.seekCur(1); // trash version byte
	img->_width = f_read16(&file);
	img->_height = f_read16(&file);
	img->frames = f_read16(&file);
	uint8_t flags = file.read();
	img->colorMode = flags & 0x01 ? (ColorMode::index) : (ColorMode::rgb565);

	if (img->colorMode == ColorMode::index) {
		img->transparentColorIndex = file.read();
		img->useTransparentIndex = (bool)file.read();
	} else {
		img->transparentColor = f_read16(&file);
	}
	file.seekSet(header_size);
	
	if (!img->_buffer) {
		img->allocateBuffer();
	}
	
	valid = true;
}

bool GMV::isValid() {
	return valid;
}

bool GMV::is(Image* _img) {
	return img == _img;
}

bool GMV::initSave(char* filename) {
	// make sure our file ends in .GMV
	uint16_t name_len = strlen(filename);
	char _filename[name_len + 1];
	strcpy(_filename, filename);
	
	_filename[name_len - 4] = '.';
	_filename[name_len - 3] = 'G';
	_filename[name_len - 2] = 'M';
	_filename[name_len - 1] = 'V';
	_filename[name_len] = '\0';
	if (SD.exists(_filename) && !SD.remove(_filename)) {
		return false;
	}
	file = SD.open(_filename, FILE_WRITE);
	if (!file) {
		return false;
	}
	writeHeader();
	return true;
}

void GMV::convertFromBMP(BMP& bmp, char* newname) {
	img->allocateBuffer();
	if (SD.exists(newname) && !SD.remove(newname)) {
		return;
	}
	File f = SD.open(newname, FILE_WRITE);
	if (!f) {
		return;
	}
	writeHeader(&f);
	uint16_t transparentColor = 0;
	
	bool success;
	do {
		success = true;
		f.seekSet(header_size);
		for (uint16_t frame = 0; frame < img->frames; frame++) {
			uint32_t t = bmp.readFrame(frame, img->_buffer, transparentColor, &file);
			if (t != transparentColor) {
				if (t) {
					transparentColor = t;
					success = false;
					// trigger a restart
					break;
				}
			}
			if (img->colorMode == ColorMode::rgb565) {
				if (transparentColor != 0xFFFF) {
					img->transparentColor = transparentColor;
				} else {
					img->transparentColor = 0;
				}
			}
			writeFrame(&f);
		}
	} while(!success);
	
	if (img->colorMode == ColorMode::rgb565) {
		f.seekSet(12);
		f_write16(img->transparentColor, &f);
	}
	bmp.setCreatorBits(CONVERT_MAGIC, &file);
	f.close();
}

void GMV::writeHeader() {
	writeHeader(&file);
}

void GMV::writeFrame() {
	writeFrame(&file);
}

void GMV::writeColor(File* f, uint16_t color, uint8_t count) {
	if (count > 1) {
		count |= 0x80;
		f->write(count);
	}
	if (color == img->transparentColor) {
		f->write((uint8_t)0x7F);
		return;
	}
	uint16_t* index = (uint16_t*)img->colorIndex;
	for (uint8_t i = 0; i < 16; i++) {
		if (index[i] == color) {
			f->write(i);
			return;
		}
	}
	f->write(0x80);
	f_write16(color, f);
}

void GMV::writeHeader(File* f) {
	f->rewind();
	f_write16(0x5647, f); // header "GV"
	f_write16(0, f); // header size, fill in later
	f->write((uint8_t)0); // version
	f_write16(img->_width, f); // image width
	f_write16(img->_height, f); // image height
	f_write16(img->frames, f); // number of frames
	bool indexed = img->colorMode == ColorMode::index;
	f->write(indexed ? 1 : 0); // indexed?
	// write the transparent color!
	if (indexed) {
		f->write(img->transparentColorIndex);
		f->write((uint8_t)img->useTransparentIndex);
	} else {
		f_write16(img->transparentColor, f);
	}
	header_size = 14; // currently it is still all static
	f->seekSet(2);
	f_write16(header_size, f); // fill in header size!
	f->seekSet(header_size);
}

void GMV::writeFrame(File* f) {
	if (img->colorMode == ColorMode::index) {
		uint8_t* buf = (uint8_t*)img->_buffer;
		uint16_t bytes = img->getBufferSize();
		uint16_t i = 1;
		uint8_t b = buf[0];
		uint16_t count = 1;
		for (; i < bytes; i++) {
			if (buf[i] == b && count < 0xFF) {
				count++;
				continue;
			}
			f->write(count);
			f->write(b);
			count = 1;
			b = buf[i];
		}
		f->write(count);
		f->write(b);
	} else {
		uint16_t* buf = img->_buffer;
		uint16_t pixels = (img->getBufferSize() + 1) / 2; 
		uint16_t i = 1;
		uint16_t color = buf[0];
		uint16_t count = 1;
		for (; i < pixels; i++) {
			if (buf[i] == color && count < 0x7F) {
				count++;
				continue;
			}
			// ok we need to write stuff
			writeColor(f, color, count);
			count = 1;
			color = buf[i];
		}
		writeColor(f, color, count);
	}
}

void GMV::readFrame() {
	if (img->colorMode == ColorMode::index) {
		uint8_t* buf = (uint8_t*)img->_buffer;
		uint16_t bytes = img->getBufferSize();
		uint16_t bytes_current = 0;
		uint8_t count;
		uint8_t b;
		do {
			count = file.read();
			b = file.read();
			for (uint8_t i = 0; i < count; i++) {
				buf[bytes_current] = b;
				bytes_current++;
			}
		} while (bytes_current < bytes);
	} else {
		uint16_t* buf = img->_buffer;
		uint16_t pixels = (img->getBufferSize() + 1) / 2; 
		uint16_t pixels_current = 0;
		
		uint16_t* index = (uint16_t*)img->colorIndex;
		
		uint8_t count;
		uint8_t i;
		uint16_t color = 0;
		do {
			count = file.read();
			if (count == 0x80) {
				// we have a single, un-altered pixel
				file.read(&color, 2);
				buf[pixels_current] = color;
				pixels_current++;
				continue;
			}
			if (!(count & 0x80)) {
				// single indexed color
				if (count == 0x7F) {
					buf[pixels_current] = img->transparentColor;
				} else {
					buf[pixels_current] = index[count];
				}
				pixels_current++;
				continue;
			}
			// ok we actually have multiple pixels
			count &= 0x7F;
			i = file.read();
			if (i == 0x80) {
				file.read(&color, 2);
			} else if (i == 0x7F) {
				color = img->transparentColor;
			} else {
				color = index[i];
			}
			for (i = 0; i < count; i++) {
				buf[pixels_current] = color;
				pixels_current++;
			}
		} while (pixels_current < pixels);
	}
}

void GMV::setFrame(uint16_t frame) {
	file.seekSet(header_size);
	if (!frame) {
		return;
	}
	for (uint16_t f = 0; f < frame; f++) {
		readFrame(); // unfortunatelly we don't have a better way
	}
}

void GMV::finishSave(char* filename, uint16_t frames, bool output, Display_ST7735* tft) {
	file.seekSet(9);
	f_write16(frames, &file); // fill in the number of frames!
	if (!filename) {
		file.close();
		return;
	}
	file.flush();
	File f = SD.open(filename, FILE_WRITE);
	if (!f) {
		if (output) {
			tft->println("Couldn't create BMP!");
		}
		return;
	}
	BMP bmp = BMP(img, frames);
	uint32_t image_size = bmp.writeHeader(&f);
	uint8_t x, y;
	if (output) {
		tft->print("Total frames: ");
		tft->println(frames);
		tft->print("Creating file (");
		tft->print(image_size / 1024);
		tft->print(") ");
		x = tft->cursorX;
		y = tft->cursorY;
	}
	for (uint32_t i = 0; i < image_size; i+=4) {
		f_write32(0, &f);
		if ((i % 65536) == 0) {
			tft->cursorX = x;
			tft->cursorY = y;
			tft->print(i / 1024);
		}
	}
	if (output) {
		tft->cursorX = x;
		tft->cursorY = y;
		tft->println("done!!!");
		tft->print("Frame: ");
		x = tft->cursorX;
		y = tft->cursorY;
	}
	setFrame(0);
	for (uint16_t i = 0; i < frames; i++) {
		if (output) {
			tft->cursorX = x;
			tft->cursorY = y;
			tft->print(i+1); // +1 for human-readability
		}
		readFrame();
		bmp.writeFrame(i, img->_buffer, img->transparentColor, &f);
	}
	bmp.setCreatorBits(CONVERT_MAGIC, &f); // ok we know that we have the GMV so why not?
	f.close();
	file.close();
}

}
