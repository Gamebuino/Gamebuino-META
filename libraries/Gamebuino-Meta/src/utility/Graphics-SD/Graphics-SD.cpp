#include "Graphics-SD.h"
#include "../SdFat.h"
#include "../Misc.h"
extern SdFat SD;

namespace Gamebuino_Meta {

//Display_ST7735* tft;

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
#if 0
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

RLE_Video::RLE_Video(Image& _img, File& _file) {
	img = &_img;
	file = _file;
}

void RLE_Video::restoreFrame() {
	uint16_t* buf = img->_buffer;
	uint16_t pixels = (img->getBufferSize() + 1) / 2; 
	uint16_t pixels_current = 0;
	
	uint16_t* index = (uint16_t*)img->colorIndex;
	
	uint8_t count = 0;
	uint8_t i;
	uint16_t color = 0;
	while (pixels_current < pixels) {
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
			buf[pixels_current] = index[count];
			pixels_current++;
			continue;
		}
		// ok we actually have multiple pixels
		count &= 0x7F;
		i = file.read();
		if (i == 0x80) {
			file.read(&color, 2);
		} else {
			color = index[i];
		}
		for (i = 0; i < count; i++) {
			buf[pixels_current] = color;
			pixels_current++;
		}
	}
}

Recording_Image::Recording_Image(BMP& _bmp, File& _file, File& _file_tmp) {
	bmp = _bmp;
	file = _file;
	file_tmp = _file_tmp;
	frames = 0;
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
	RLE_Video rle = RLE_Video(*(bmp.img), file_tmp);
	for (uint32_t i = 0; i < frames; i++) {
		if (output) {
			tft->cursorX = x;
			tft->cursorY = y;
			tft->print(i+1); // +1 for human-readability
		}
		rle.restoreFrame();
		bmp.writeFrame(i, file);
	}
	file.close();
	file_tmp.close();
//	file_tmp.remove(); // we don't need you anymore!
}

bool Recording_Image::is(Image* img) {
	return bmp.img == img;
}

Playing_Image::Playing_Image(RLE_Video& _rle) {
	rle = _rle;
}

void Playing_Image::update() {
	rle.restoreFrame();
	
	if (rle.file.peek() == -1) {
		rle.file.rewind();
	}
}

bool Playing_Image::is(Image* img) {
	return rle.img == img;
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
	BMP bmp = BMP(file, &img);
	if (!bmp.isValid()) {
		file.close();
		return false;
	}
	bmp.readBuffer(file);
	return true;
}

void Gamebuino_SD_GFX::update() {
	for (uint8_t i = 0; i < MAX_IMAGE_RECORDING; i++) {
		if (recording[i]) {
			recording[i]->update();
		}
	}
	
	for (uint8_t i = 0; i < MAX_IMAGE_PLAYING; i++) {
		if (playing[i]) {
			playing[i]->update();
		}
	}
}

bool Gamebuino_SD_GFX::playImage(Image &img, char *filename) {
	uint8_t i = 0;
	for (; i < MAX_IMAGE_PLAYING; i++) {
		if (!playing[i]) {
			break;
		}
	}
	if (i == MAX_IMAGE_PLAYING) {
		return false; // no empty slot
	}
	File file = SD.open(filename);
	if (!file) {
		// file doesn't exist
		return false;
	}
	/*BMP bmp = BMP(file, &img);
	if (!bmp.isValid()) {
		file.close();
		return false;
	}*/
	file.rewind();
	RLE_Video rle = RLE_Video(img, file);
	Playing_Image* play = new Playing_Image(rle);
	play->update(); // we want to be able to use it right away!
	playing[i] = play;
	
	return true;
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
	
	char tmp_name[] = "/TMP0000.BIN";
	if (!sd_path_no_duplicate(tmp_name, 4, 4)) {
		return false;
	}
	File file_tmp = SD.open(tmp_name, FILE_WRITE);
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
		return; // image not found
	}
	recording[i]->finish(output);
	
	delete recording[i];
	recording[i] = 0;
}

void Gamebuino_SD_GFX::stopRecordImage(Image& img, Display_ST7735& _tft) {
	tft = &_tft;
	stopRecordImage(img, true);
}
#endif

Frame_Handler_SD::Frame_Handler_SD(Image* img) : Frame_Handler(img) {
	
}

void Frame_Handler_SD::init(char* filename) {
	gmv = new GMV(img, filename);
	if (!gmv->isValid()) {
		if (img->frames == 0) {
			delete gmv;
			lazy_filename = filename;
		}
		return;
	}
	if (img->frames == 1) {
		gmv->readFrame();
		delete gmv; // no need to keep you
		gmv = 0;
	}
}

Frame_Handler_SD::~Frame_Handler_SD() {
	deallocateBuffer();
	if (gmv) {
		delete gmv;
	}
}

void Frame_Handler_SD::lazy_init() {
	if (img->frames) {
		// we already inited
		return;
	}
	init(lazy_filename);
	if (!gmv->isValid()) {
		// we are lost
		delete gmv;
		gmv = 0;
		img->frames = 1;
	}
}

void Frame_Handler_SD::next() {
	lazy_init();
	if (gmv) {
		gmv->readFrame();
	}
}

void Frame_Handler_SD::set(uint16_t frame) {
	lazy_init();
	if (gmv) {
		gmv->setFrame(frame);
		gmv->readFrame();
	}
}

} // namespace Gamebuino_Meta
