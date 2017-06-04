#include "BMP.h"
#include "../Misc.h"

namespace Gamebuino_Meta {

uint16_t convertTo565(uint8_t r, uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

BMP::BMP() {
	valid = false;
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
		img->frames = pixel_height / img->_height;
	} else {
		img->_height = pixel_height;
		img->frames = 1;
	}
	if (f_read16(file) != 1) { // # planes, must always be 1
		return;
	}
	uint16_t depth = f_read16(file);
	if (depth != 4 && depth != 24) {
		// we can only load uncompressed BMPs
		return;
	}
	indexed = depth == 4;
	if (f_read32(file) != 0) {
		// we can only load uncompressed BMPs
		return;
	}
	//file->seekCur(12); // we ignore image size, x pixels and y pixels per meter
	//colorTable = f_read32(file);
	// we assume our color table so just ignore the one specified in the BMP
	file->seekSet(image_offset);
	
	if (indexed) {
		img->colorMode = ColorMode::index;
	} else {
		img->colorMode = ColorMode::rgb565;
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

void BMP::setCreatorBits(File* file, uint32_t bits) {
	file->seekSet(6);
	f_write32(bits, file);
}

/*
void BMP::setFrames(uint32_t _frames) {
	frames = _frames;
	pixel_height = img->_height*frames;
	imageSize = width*pixel_height; // this holds the image size in bytes
	fileSize = imageOffset + imageSize; // this is the filesize
}
*/

/*
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
*/

/*
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
*/

/*
void BMP::writeFrame(uint32_t frame, File& file) {
	uint32_t size = width * img->_height;
	uint32_t offset = size * (frames - frame - 1);
	file.seekSet(imageOffset + offset);
	writeBuffer(file);
}
*/

uint32_t BMP::getRowSize() {
	if (indexed) {
		return ((4*width+31)/32) * 4;
	}
	return (width * 3 + 3) & ~3;
}

void BMP::readBuffer(File* file, uint16_t* buf, uint16_t height, uint32_t offset) {
	
	uint32_t rowSize = getRowSize();
	file->seekSet(offset);
	if (indexed) {
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
				rambuffer[j] = convertTo565(r, g, b);
			}
			file->seekCur(dif);
		}
	}
}

void BMP::readFrame(uint16_t frame, uint16_t frames, uint16_t height, File* file, uint16_t* buf) {
	uint32_t size = getRowSize() * height;
	uint32_t offset = size * (frames - frame - 1);
	readBuffer(file, buf, height, image_offset + offset);
}

} // namespace Gamebuino_Meta
