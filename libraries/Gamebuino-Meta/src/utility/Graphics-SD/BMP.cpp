#include "BMP.h"
#include "../Misc.h"

namespace Gamebuino_Meta {

uint16_t convertTo565(uint8_t r, uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void writeAsRGB(uint16_t b, File* f) {
	uint8_t c = (uint8_t)(b << 3);
	c |= c >> 5;
	f->write(c);
	
	// green
	c = (uint8_t)((b >> 3) & 0xFC);
	c |= c >> 6;
	f->write(c);
	
	// blue
	c = (uint8_t)((b >> 8) & 0xF8);
	c |= c >> 5;
	f->write(c);
}

BMP::BMP() {
	valid = false;
}

BMP::BMP(Image* img, uint16_t _frames) {
	if (img->colorMode == ColorMode::index) {
		depth = 4;
	} else {
		depth = 24; // TODO: 32-bit depth with transparency
	}
	width = img->_width;
	height = img->_height;
	frames = _frames;
}

BMP::BMP(File* file, Image* img) {
	valid = false;
	file->rewind();
	if (f_read16(file) != 0x4D42) {
		// no valid BMP header
		return;
	}
	file->seekCur(4); // skip file size
	creatorBits = f_read32(file);
	image_offset = f_read32(file);
	file->seekCur(4); // skip header size
	width = img->_width = f_read32(file);
	uint32_t pixel_height = f_read32(file);
	if (img->_height) {
		height = img->_height;
		img->frames = frames = pixel_height / img->_height;
	} else {
		img->_height = height = pixel_height;
		img->frames = frames = 1;
	}
	if (f_read16(file) != 1) { // # planes, must always be 1
		return;
	}
	depth = f_read16(file);
	if (depth != 4 && depth != 24 && depth != 32) {
		// we can only load BMPs of depth 4 or 24 or 32
		return;
	}
	if (f_read32(file) != 0) {
		// we can only load uncompressed BMPs
		return;
	}
	//file->seekCur(12); // we ignore image size, x pixels and y pixels per meter
	//colorTable = f_read32(file);
	// we assume our color table so just ignore the one specified in the BMP
	file->seekSet(image_offset);
	
	if (depth == 4) {
		img->colorMode = ColorMode::index;
		img->useTransparentIndex = false; // TODO: transparency detection
	} else {
		img->colorMode = ColorMode::rgb565;
		img->transparentColor = 0; // TODO: transparency detection
	}
	//img->allocateBuffer();
	valid = true;
}

bool BMP::isValid() {
	return valid;
}

uint32_t BMP::getCreatorBits() {
	return creatorBits;
}

void BMP::setCreatorBits(uint32_t bits, File* file) {
	file->seekSet(6);
	f_write32(bits, file);
}

uint32_t BMP::writeHeader(File* file) {
	// lets first create some helpful variables
	uint16_t header_size = 40;
	uint32_t pixel_height = height * frames;
	uint8_t color_table = 0;
	if (depth == 4) {
		color_table = 16;
	}
	image_offset = 14 + header_size + color_table * 4;
	uint32_t image_size = getRowSize() * pixel_height;
	uint32_t file_size = image_offset + image_size;
	
	
	file->rewind();
	file->write("BM"); // this actually is a BMP image
	f_write32(file_size, file);
	f_write32(0, file); // creator bits
	f_write32(image_offset, file);
	f_write32(header_size, file);
	f_write32(width, file);;
	f_write32(pixel_height, file);
	f_write16(1, file); // number of panes must be 1
	f_write16(depth, file);
	f_write32(0, file); // no compression
	f_write32(image_size, file);
	f_write32(0, file); // x pixels per meter horizontal
	f_write32(0, file); // y pixels per meter vertically
	f_write32(color_table, file);
	if (color_table) {
		f_write32(color_table, file); // important colors
		for (uint8_t i = 0; i < color_table; i++) {
			writeAsRGB((uint16_t)Graphics::colorIndex[i], file);
			file->write((uint8_t)0);
		}
	} else {
		f_write32(0, file); // no important colors
	}
	return image_size;
}

uint32_t BMP::getRowSize() {
	if (depth == 4) {
		return ((4*width+31)/32) * 4;
	}
	if (depth == 24) {
		return (width * 3 + 3) & ~3;
	}
	return width * 4;
}

void BMP::readBuffer(uint16_t* buf, uint32_t offset, File* file) {
	
	uint32_t rowSize = getRowSize();
	file->seekSet(offset);
	if (depth == 4) {
		uint8_t dif = rowSize - ((width + 1) / 2);
		
		for (uint16_t i = 0; i < height; i++) {
			uint8_t* rambuffer = (uint8_t*)buf + (height - 1 - i) * ((width + 1) / 2);
			
			
			for (uint16_t j = 0; j < (width + 1)/2; j++) {
				rambuffer[j] = file->read();
			}
			file->seekCur(dif);
		}
	} else {
		uint8_t dif = rowSize - (width * 3);
		for (uint16_t i = 0; i < height; i++) {
			uint16_t* rambuffer = buf + (height - 1 - i) * width;
			
			for (uint16_t j = 0; j < width; j++) {
				uint8_t b = file->read();
				uint8_t g = file->read();
				uint8_t r = file->read();
				if (depth == 32) {
					file->read(); // trash alpha chanel
				}
				rambuffer[j] = convertTo565(r, g, b);
			}
			if (depth != 32) { // 32-bit always has dif 0
				file->seekCur(dif);
			}
		}
	}
}

void BMP::readFrame(uint16_t frame, uint16_t* buf, File* file) {
	uint32_t size = getRowSize() * height;
	uint32_t offset = size * (frames - frame - 1);
	readBuffer(buf, image_offset + offset, file);
}

void BMP::writeBuffer(uint16_t* buffer, File* file) {
	if (depth == 4) {
		uint8_t halfwidth = (width + 1) / 2;
		uint8_t j = getRowSize() - halfwidth;
		for (int8_t y = height - 1; y >= 0; y--) {
			uint8_t* buf = (uint8_t*)buffer + y*halfwidth;
			for (uint8_t x = 0; x < halfwidth; x++) {
				file->write(buf[x]);
			}
			uint8_t i = j;
			while (i--) {
				// time to add padding
				file->write((uint8_t)0);
			}
		}
	} else {
		uint8_t j = getRowSize() - (3*width);
		for (int8_t y = height - 1; y >= 0; y--) {
			uint16_t* buf = buffer + (y*width);
			for (uint8_t x = 0; x < width; x++) {
				writeAsRGB(buf[x], file);
			}
			uint8_t i = j;
			while (i--) {
				// time to add padding
				file->write((uint8_t)0);
			}
		}
	}
}

void BMP::writeFrame(uint16_t frame, uint16_t* buf, File* file) {
	uint32_t size = getRowSize() * height;
	uint32_t offset = size * (frames - frame - 1);
	file->seekSet(image_offset + offset);
	writeBuffer(buf, file);
}

} // namespace Gamebuino_Meta
