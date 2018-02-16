/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2017

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

#ifndef _GAMEBUINO_GMV_H_
#define	_GAMEBUINO_GMV_H_

#include "BMP.h"
#include "../Image.h"
#include "../SdFat.h"
#include "../Display-ST7735.h"
#include "../Graphics.h"

namespace Gamebuino_Meta {

class GMV {
public:
	GMV();
	GMV(Image* _img);
	GMV(Image* _img, char* filename);
	bool isValid();
	bool is(Image* _img);
	bool initSave(char* filename);
	uint16_t getFrames();
	void writeHeader();
	void writeFrame();
	void readFrame();
	void setFrame(uint16_t frame);
	void finishSave(char* filename, uint16_t frames, bool output, Display_ST7735* tft);
private:
	File file;
	Image* img;
	void convertFromBMP(BMP& bmp, char* newname);
	void writeColor(File* f, uint16_t color, uint8_t count);
	void writeHeader(File* f);
	void writeFrame(File* f);
	bool valid;
	uint16_t header_size;
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_GMV_H_
