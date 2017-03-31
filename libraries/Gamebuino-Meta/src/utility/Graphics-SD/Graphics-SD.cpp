#include "Graphics-SD.h"
#include "../SdFat.h"
extern SdFat SD;

namespace Gamebuino_Meta {

Print* Gamebuino_SD_GFX::debugOutput = 0;

template <typename T>
inline void printDebug(T output){
	if(!Gamebuino_SD_GFX::debugOutput) return;
	Gamebuino_SD_GFX::debugOutput->print(output);
}

template <typename T>
inline void printlnDebug(T output){
	if(!Gamebuino_SD_GFX::debugOutput) return;
	Gamebuino_SD_GFX::debugOutput->println(output);
}

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


void Gamebuino_SD_GFX::begin(){
	
}

uint8_t Gamebuino_SD_GFX::writeImage(Image& img, char *filename){
	printDebug("SAVING TO ");
	printlnDebug(filename);

	// let's first make sure that our file doesn't already exist
	if (SD.exists(filename)) {
		printDebug(" ALREADY EXISTS, REMOVING");
		if (SD.remove(filename)) {
			printlnDebug(" OK");
		} else {
			printlnDebug(" FAILED");
			return 1;
		}
	}

	printDebug(" CREATING FILE ");
	File file = SD.open(filename, FILE_WRITE);
	if (!file) {
		printlnDebug(" FAILED");
		return 2;
	}
	printlnDebug(" OK");
	
	uint8_t bmpDepth; // let's generate our bit-depth
#define BMP_HEADER_SIZE 40
	uint32_t bmpWidth; // this will hold the bmp width, padded to four bytes
	uint32_t colorTable = 0; // this is the color table, if needed
	uint16_t* rambuffer = img._buffer;
	switch(img.colorMode){
		case ColorMode::index: 
			bmpDepth=4;
			bmpWidth = ((bmpDepth*img._width + 31)/32) * 4;
			colorTable = 16; // 4-bit index colors
			
			printlnDebug("Indexed colors");
			break;
		case ColorMode::rgb565: 
			bmpDepth=24;
			bmpWidth = (img._width * 3 + 3) & ~3;
			
			printlnDebug("Full colors");
			break;
		default:
			printDebug("Invalid Image mode");
			return 255;
	}
	uint32_t bmpImageoffset = 14 + BMP_HEADER_SIZE + colorTable * 4; // here the image will start
	uint32_t bmpImageSize = bmpWidth*img._height; // this holds the image size in bytes
	uint32_t fileSize = bmpImageoffset + bmpImageSize; // this is the filesize
	
	file.truncate(0);
	// let's start writing the BMP header!
	file.write("BM"); // this actually is a BMP image
	printDebug('.');
	write32(fileSize, file);
	write32(0, file); // reserved
	printDebug('.');
	write32(bmpImageoffset, file);
	printDebug('.');
	write32(BMP_HEADER_SIZE, file);
	printDebug('.');
	write32(img._width, file);
	printDebug('.');
	write32(img._height, file);
	printDebug('.');
	write16(1, file); // planes must be 1
	write16(bmpDepth, file);
	write32(0, file); // no compression
	printDebug('.');
	write32(bmpImageSize, file);
	printDebug('.');
	write32(0, file); // x pixels per meter horizontal
	write32(0, file); // y pixels per meter vertical
	write32(colorTable, file); // number of colors in the color table
	if (colorTable) {
		// we have a color table
		write32(colorTable, file); // important colors
		for (uint32_t i = 0; i < colorTable; i++) {
			writeAsRGB((uint16_t)img.colorIndex[i], file);
			
			file.write((uint8_t)0);
			printDebug('.');
		}
		uint8_t j = bmpWidth - (img._width / 2);
		for (int8_t y = img._height - 1; y >= 0; y--) {
			uint8_t* buf = (uint8_t*)rambuffer + (y*img._width)/2;
			for (uint8_t x = 0; x < img._width/2; x++) {
				file.write(*(buf++));
			}
			for (uint8_t i = j; i > 0; i++) {
				// time to add padding
				file.write((uint8_t)0);
			}
			printDebug('.');
		}
	} else {
		// we don't have a color table
		write32(0, file); // no important colors
		printDebug(".\n");
		// the header is done now, it is time to output the pixels!
		uint8_t j = bmpWidth - (3*img._width);
		for (int8_t y = img._height - 1; y >= 0; y--) {
			uint16_t* buf = rambuffer + (y*img._width);
			for (uint8_t x = 0; x < img._width; x++) {
				writeAsRGB(buf[x], file);
			}
			uint8_t i = j;
			while (i--) {
				// time to add padding
				file.write((uint8_t)0);
			}
			printDebug('.');
		}
	}
	file.close();
	printDebug("\n DONE!");
	return 0;
}

uint8_t Gamebuino_SD_GFX::readImage(Image& img, char *filename){
	printlnDebug("Opening file...");
	File file = SD.open(filename);
	if (!file) {
		printDebug("error opening ");
		printlnDebug(filename);
		return 1;
	}
	file.rewind();
	if (read16(file) != 0x4D42) {
		printlnDebug("File isn't a BMP file!");
		return 1;
	}
	file.seekCur(8); // skip filesize and creator bits
	uint32_t bmpImageoffset = read32(file);
	file.seekCur(4); // skip header size
	int32_t bmpWidth = (int32_t)read32(file);
	int32_t bmpHeight = (int32_t)read32(file);
	
	if (read16(file) != 1) { // # planes, must always be 1
		printlnDebug("Bad # of planes");
		return 1;
	}
	uint16_t bmpDepth = read16(file);
	printDebug("Bit Depth: ");
	printlnDebug(bmpDepth);
	printDebug("Image size: ");
	printDebug(bmpWidth);
	printDebug(" x ");
	printlnDebug(bmpHeight);
	if (read32(file) != 0) {
		printlnDebug("ERROR: can load only uncompressed BMPs");
		return 1;
	}
	if (bmpDepth != 4 && bmpDepth != 24) {
		printlnDebug("ERROR: can only load BMPs with img depth 4 or 24");
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
					printlnDebug("ERROR: file is too small");
					return 1;
				}
				*(rambuffer++) = convertTo565(r, g, b);
			}
		}
		return 0;
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
		for (uint16_t j = 0; j < bmpWidth/2; j++) {
			*(rambuffer++) = file.read();
		}
	}
	return 0;
}

} // namespace Gamebuino_Meta
