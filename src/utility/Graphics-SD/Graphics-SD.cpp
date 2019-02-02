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
 - Sorunome
*/

#include "Graphics-SD.h"
#include "../Misc.h"

#if USE_SDFAT
#include "../SdFat.h"
extern SdFat SD;
#endif

namespace Gamebuino_Meta {


Recording_Image* recording_images[MAX_IMAGE_RECORDING];
Display_ST7735* tft;

Frame_Handler_SD::Frame_Handler_SD(Image* img) : Frame_Handler(img) {
	gmv = 0;
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
	if (img->frames && gmv) {
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


Recording_Image::Recording_Image(GMV& _gmv) {
	gmv = _gmv;
	frames = 0;
	bmp_filename = 0;
}

Recording_Image::~Recording_Image() {
	if (bmp_filename) {
		gb_free(bmp_filename);
	}
}

void Recording_Image::update() {
	gmv.writeFrame();
	frames++;
}

bool Recording_Image::is(Image* img) {
	return gmv.is(img);
}

void Recording_Image::finish(bool output) {
	gmv.finishSave(bmp_filename, frames, output, tft);
}

void Recording_Image::setBmpFilename(char* filename) {
#if USE_SDFAT
	bmp_filename = (char*)gb_malloc(strlen(filename) + 1);
	strcpy(bmp_filename, filename);
	File f = SD.open(bmp_filename, FILE_WRITE);
	f.write((uint8_t)0); // make sure we already create it
	f.close();
#endif // USE_SDFAT
}


bool Graphics_SD::startRecording(Image* img, char* filename) {
#if USE_SDFAT
	uint8_t i = 0;
	for (; i < MAX_IMAGE_RECORDING; i++) {
		if (!recording_images[i]) {
			break;
		}
	}
	if (i == MAX_IMAGE_RECORDING) {
		return false; // no empty slot
	}
	bool convert_bmp = true;
	if (convert_bmp && SD.exists(filename) && !SD.remove(filename)) {
		return false;
	}
	GMV gmv = GMV(img);
	if (!gmv.initSave(filename)) {
		return false;
	}
	Recording_Image* rec = new Recording_Image(gmv);
	if (convert_bmp && !SD.exists(filename)) {
		rec->setBmpFilename(filename);
	}
	recording_images[i] = rec;
	return true;
#else // USE_SDFAT
	return false;
#endif
}

void Graphics_SD::stopRecording(Image* img, bool output) {
#if USE_SDFAT
	uint8_t i = 0;
	for (; i < MAX_IMAGE_RECORDING; i++) {
		if (!recording_images[i]) {
			continue;
		}
		if (recording_images[i]->is(img)) {
			break;
		}
	}
	if (i == MAX_IMAGE_RECORDING) {
		return; // image not found
	}
	recording_images[i]->finish(output);
	delete recording_images[i];
	recording_images[i] = 0;
#endif // USE_SDFAT
}

bool Graphics_SD::save(Image* img, char* filename) {
#if USE_SDFAT
	bool convert_bmp = true; // for saving single frames we always convert
	if (convert_bmp && SD.exists(filename) && !SD.remove(filename)) {
		return false;
	}
	GMV gmv = GMV(img);
	if (!gmv.initSave(filename)) {
		return false;
	}
	Recording_Image rec = Recording_Image(gmv);
	if (convert_bmp && !SD.exists(filename)) {
		rec.setBmpFilename(filename);
	}
	rec.update();
	rec.finish(false);
	return true;
#else // USE_SDFAT
	return false;
#endif // USE_SDFAT
}

void Graphics_SD::update() {
	for (uint8_t i = 0; i < MAX_IMAGE_RECORDING; i++) {
		if (recording_images[i]) {
			recording_images[i]->update();
		}
	}
}

void Graphics_SD::setTft(Display_ST7735* _tft) {
	tft = _tft;
}

} // namespace Gamebuino_Meta
