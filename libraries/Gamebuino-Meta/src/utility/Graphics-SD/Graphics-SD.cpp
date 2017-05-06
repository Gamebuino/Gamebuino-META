#include "Graphics-SD.h"
#include "../SdFat.h"
extern SdFat SD;

namespace Gamebuino_Meta {

Display_ST7735* tft;

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
uint16_t read16(File& f) {
	uint16_t result;
	f.read(&result, 2);
	return result;
}

uint32_t read32(File& f) {
	uint32_t result;
	f.read(&result, 4);
	return result;
}

uint16_t convertTo565(uint8_t r, uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void writeAsRGB(uint16_t b, File& f) {
	uint8_t c = (uint8_t)(b << 3);
	c |= c >> 5;
	f.write(c);
	
	// green
	c = (uint8_t)((b >> 3) & 0xFC);
	c |= c >> 6;
	f.write(c);
	
	// blue
	c = (uint8_t)((b >> 8) & 0xF8);
	c |= c >> 5;
	f.write(c);
}

void write32(uint32_t b, File& f) {
	//Write four bytes
	//Luckily our MCU is little endian so byte order like this is fine
	f.write(&b, 4);
}

void write16(uint16_t b, File& f) {
	//Write two bytes
	//Luckily our MCU is little endian so byte order like this is fine
	f.write(&b, 2);
}

BMP::BMP(Image* _img) {
	valid = false;
	
	img = _img;
	header_size = 40;
	colorTable = 0;
	rambuffer = img->_buffer;
	switch (img->colorMode) {
		case ColorMode::index: 
			depth=4;
			width = ((depth*img->_width + 31)/32) * 4;
			colorTable = 16; // 4-bit index colors
			break;
		case ColorMode::rgb565: 
			depth=24;
			width = (img->_width * 3 + 3) & ~3;
			break;
		default:
			// unkown / invalid color mode
			return;
	}
	pixel_height = img->_height;
	imageOffset = 14 + header_size + colorTable * 4; // here the image will start
	imageSize = width*pixel_height; // this holds the image size in bytes
	fileSize = imageOffset + imageSize; // this is the filesize
	
	valid = true; // everything seems OK!
}

bool BMP::isValid() {
	return valid;
}

void BMP::setFrames(uint32_t frames) {
	pixel_height = img->_height*frames;
	imageSize = width*pixel_height; // this holds the image size in bytes
	fileSize = imageOffset + imageSize; // this is the filesize
}

void BMP::writeHeader(File& file) {
	file.rewind();
	file.write("BM"); // this actually is a BMP image
	write32(fileSize, file);
	write32(0, file); // reserved
	write32(imageOffset, file);
	write32(header_size, file);
	write32(img->_width, file); // pixel width
	write32(pixel_height, file); // pixel height
	write16(1, file); // planes must be 1
	write16(depth, file);
	write32(0, file); // no compression
	write32(imageSize, file);
	write32(0, file); // x pixels per meter horizontal
	write32(0, file); // y pixels per meter vertical
	write32(colorTable, file); // number of colors in the color table
	if (colorTable) {
		// we have a color table
		write32(colorTable, file); // important colors
		for (uint32_t i = 0; i < colorTable; i++) {
			writeAsRGB((uint16_t)img->colorIndex[i], file);
			
			file.write((uint8_t)0);
		}
	} else {
		write32(0, file); // no important colors
	}
	file.truncate(fileSize);
}

void BMP::writeBuffer(File& file) {
	if (colorTable) {
		uint8_t halfwidth = (img->_width + 1) / 2;
		uint8_t j = width - halfwidth;
		for (int8_t y = img->_height - 1; y >= 0; y--) {
			uint8_t* buf = (uint8_t*)rambuffer + y*halfwidth;
			for (uint8_t x = 0; x < halfwidth; x++) {
				file.write(buf[x]);
			}
			uint8_t i = j;
			while (i--) {
				// time to add padding
				file.write((uint8_t)0);
			}
		}
	} else {
		uint8_t j = width - (3*img->_width);
		for (int8_t y = img->_height - 1; y >= 0; y--) {
			uint16_t* buf = rambuffer + (y*img->_width);
			for (uint8_t x = 0; x < img->_width; x++) {
				writeAsRGB(buf[x], file);
			}
			uint8_t i = j;
			while (i--) {
				// time to add padding
				file.write((uint8_t)0);
			}
		}
	}
}

void BMP::writeFrame(uint32_t frame, uint32_t frames, File& file) {
	uint32_t size = width * img->_height;
	uint32_t offset = size * (frames - frame - 1);
	file.seekSet(imageOffset + offset);
	writeBuffer(file);
}

Recording_Image::Recording_Image(BMP& _bmp, File& _file, File& _file_tmp) {
	bmp = _bmp;
	file = _file;
	file_tmp = _file_tmp;
	frames = 0;
}

void Recording_Image::writeColor(uint16_t color, uint8_t count) {
	if (count > 1) {
		count |= 0x80;
		file_tmp.write(count);
	}
	uint16_t* index = (uint16_t*)bmp.img->colorIndex;
	for (uint8_t i = 0; i < 16; i++) {
		if (index[i] == color) {
			file_tmp.write(i);
			return;
		}
	}
	file_tmp.write(0x80);
	file_tmp.write(&color, 2);
}

void Recording_Image::update() {
	uint16_t pixels = (bmp.img->getBufferSize() + 1) / 2; 
	uint16_t i = 1;
	uint16_t* buf = bmp.img->_buffer;
	uint16_t color = buf[i];
	uint16_t count = 1;
	for (; i < pixels; i++) {
		if (buf[i] == color && count < 0x7F) {
			count++;
			continue;
		}
		// ok we need to write stuff
		writeColor(color, count);
		count = 1;
		color = buf[i];
	}
	writeColor(color, count);
	frames++;
}

void Recording_Image::restoreFrame() {
	uint16_t* buf = bmp.img->_buffer;
	uint16_t pixels = (bmp.img->getBufferSize() + 1) / 2; 
	uint16_t pixels_current = 0;
	
	uint16_t* index = (uint16_t*)bmp.img->colorIndex;
	
	uint8_t count = 0;
	uint8_t i;
	uint16_t color = 0;
	while (pixels_current < pixels) {
		count = file_tmp.read();
		if (count == 0x80) {
			// we have a single, un-altered pixel
			file_tmp.read(&color, 2);
			buf[pixels_current] = color;
			pixels_current++;
			continue;
		}
		if (!(count & 0x80)) {
			// single indexed color
			buf[pixels_current] = index[count];
			pixels_current++;
			continue;
		}
		// ok we actually have multiple pixels
		count &= 0x7F;
		i = file_tmp.read();
		if (i == 0x80) {
			file_tmp.read(&color, 2);
		} else {
			color = index[i];
		}
		for (i = 0; i < count; i++) {
			buf[pixels_current] = color;
			pixels_current++;
		}
	}
}

void Recording_Image::finish(bool output) {
	update(); // save the current frame
	file_tmp.rewind(); // we'll want to start from the beginning
	bmp.setFrames(frames);
	bmp.writeHeader(file);
	
	uint8_t x, y;
	if (output) {
		tft->print("Total frames: ");
		tft->println(frames);
		tft->print("Creating file (");
		tft->print(bmp.imageSize / 1024);
		tft->print(") ");
		x = tft->cursorX;
		y = tft->cursorY;
	}
	uint32_t zero = 0;
	for (uint32_t i = 0; i < bmp.imageSize; i+=4) {
		file.write(&zero, 4);
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
	for (uint32_t i = 0; i < frames; i++) {
		if (output) {
			tft->cursorX = x;
			tft->cursorY = y;
			tft->print(i+1); // +1 for human-readability
		}
		restoreFrame();
		bmp.writeFrame(i, frames, file);
	}
	file.close();
	file_tmp.remove(); // we don't need you anymore!
}

bool Recording_Image::is(Image* img) {
	return bmp.img == img;
}

bool Gamebuino_SD_GFX::writeImage(Image& img, char *filename) {
	// let's first make sure that our file doesn't already exist
	if (SD.exists(filename) && !SD.remove(filename)) {
		return false;
	}

	// and now create the file
	File file = SD.open(filename, FILE_WRITE);
	if (!file) {
		return false;
	}
	BMP bmp = BMP(&img);
	if (!bmp.isValid()) {
		SD.remove(filename);
		return false;
	}
	file.truncate(0);
	bmp.writeHeader(file);
	bmp.writeBuffer(file);
	file.close();
	return true;
}

bool Gamebuino_SD_GFX::readImage(Image& img, char *filename){
	File file = SD.open(filename);
	if (!file) {
		// file doesn't exist
		return false;
	}
	file.rewind();
	if (read16(file) != 0x4D42) {
		// file isn't a BMP file
		return false;
	}
	file.seekCur(8); // skip filesize and creator bits
	uint32_t bmpImageoffset = read32(file);
	file.seekCur(4); // skip header size
	int32_t bmpWidth = (int32_t)read32(file);
	int32_t bmpHeight = (int32_t)read32(file);
	
	if (read16(file) != 1) { // # planes, must always be 1
		return false;
	}
	uint16_t bmpDepth = read16(file);
	
	if (read32(file) != 0) {
		// we can only load uncompressed BMPs
		return false;
	}
	if (bmpDepth != 4 && bmpDepth != 24) {
		// only color depth of 4 or 24 is valid
		return false;
	}
	// it seems like we have a valid bitmap!
	bool flip = bmpHeight>=0; // bitmaps are stored bottom-to-top for some weird reason......I wonder who came up with that......oooooh it seems like the BMP standard originates from microsoft, that might explain some things.....NO THIS COMMENT IS NOT TOO LONG! Nor unrelated
	if (!flip) {
		bmpHeight = -bmpHeight;
	}
	uint32_t rowSize;
	if (bmpDepth == 24) {
		// we have an RGB565 image!
		rowSize = (bmpWidth * 3 + 3) & ~3;
		img.colorMode = ColorMode::rgb565;
		img.allocateBuffer(bmpWidth, bmpHeight);
		uint16_t* rambuffer = img._buffer;
		for (uint16_t i = 0; i < bmpHeight; i++) {
			uint32_t pos;
			if (flip) {
				pos = bmpImageoffset + (bmpHeight - 1 - i) * rowSize;
			} else {
				pos = bmpImageoffset + rowSize * i;
			}
			file.seekSet(pos);
			for (uint16_t j = 0; j < bmpWidth; j++) {
				int16_t b = file.read();
				int16_t g = file.read();
				int16_t r = file.read();
				if (b < 0 || g < 0 || r < 0) {
					// file is too small
					return false;
				}
				*(rambuffer++) = convertTo565(r, g, b);
			}
		}
		file.close();
		return true;
	}
	// OK we need to load an indexed BMP instead
	file.seekCur(12); // we ignore image size, x pixels and y pixels per meter
	
	// fetch the bmpColorTable but we use rowSize to re-use variables
	rowSize = read32(file);
	file.seekCur(4 + min(16, rowSize)*4); // disgard important Colors and colortable (we assume our intern color table)
	rowSize = ((bmpDepth*bmpWidth+31)/32) * 4;
	
	img.colorMode = ColorMode::index;
	img.allocateBuffer(bmpWidth, bmpHeight);
	uint8_t* rambuffer = (uint8_t*)img._buffer;
	for (uint16_t i = 0; i < bmpHeight; i++) {
		uint32_t pos;
		if (flip) {
			pos = bmpImageoffset + (bmpHeight - 1 - i) * rowSize;
		} else {
			pos = bmpImageoffset + i * rowSize;
		}
		file.seekSet(pos);
		
		for (uint16_t j = 0; j < (img._width + 1)/2; j++) {
			*(rambuffer++) = file.read();
		}
	}
	file.close();
	return true;
}

void Gamebuino_SD_GFX::update() {
	for (uint8_t i = 0; i < MAX_IMAGE_RECORDING; i++) {
		if (recording[i]) {
			recording[i]->update();
		}
	}
}

bool Gamebuino_SD_GFX::startRecordImage(Image &img, char *filename) {
	uint8_t i = 0;
	for (; i < MAX_IMAGE_RECORDING; i++) {
		if (!recording[i]) {
			break;
		}
	}
	if (i == MAX_IMAGE_RECORDING) {
		return false; // no empty slot
	}
	if (SD.exists(filename) && !SD.remove(filename)) {
		return false;
	}

	// and now create the file
	File file = SD.open(filename, FILE_WRITE);
	if (!file) {
		return false;
	}
	BMP bmp = BMP(&img);
	if (!bmp.isValid()) {
		SD.remove(filename);
		return false;
	}
	file.truncate(0);
	
	File file_tmp = SD.open("/TMP.BIN", FILE_WRITE);
	if (!file_tmp) {
		return false;
	}
	file_tmp.truncate(0);
	
	Recording_Image* rec = new Recording_Image(bmp, file, file_tmp);
	recording[i] = rec;
	
	return true;
}

void Gamebuino_SD_GFX::stopRecordImage(Image &img, bool output = false) {
	uint8_t i = 0;
	for (; i < MAX_IMAGE_RECORDING; i++) {
		if (!recording[i]) {
			continue;
		}
		if (recording[i]->is(&img)) {
			break;
		}
	}
	if (i == MAX_IMAGE_RECORDING) {
		while(1);
		return; // image not found
	}
	recording[i]->finish(output);
	
	delete recording[i];
}

void Gamebuino_SD_GFX::stopRecordImage(Image& img, Display_ST7735& _tft) {
	tft = &_tft;
	stopRecordImage(img, true);
}

} // namespace Gamebuino_Meta
