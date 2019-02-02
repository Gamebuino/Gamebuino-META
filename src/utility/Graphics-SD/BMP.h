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

#ifndef _GAMEBUINO_BMP_H_
#define	_GAMEBUINO_BMP_H_

#include "../../config/config.h"
#include "../Image.h"

#if USE_SDFAT
#include "../SdFat.h"
#endif

namespace Gamebuino_Meta {


class BMP {
public:
	BMP();
	BMP(Image* img, uint16_t frames);
#if USE_SDFAT
	BMP(File* file, Image* img);
#endif
	bool isValid();
	uint32_t getCreatorBits();
#if USE_SDFAT
	void setCreatorBits(uint32_t bits, File* file);
	uint16_t readBuffer(uint16_t* buf, uint32_t offset, uint16_t transparentColor, File* file);
	uint16_t readFrame(uint16_t frame, uint16_t* buf, uint16_t transparentColor, File* file);
#endif
	uint32_t getRowSize();
	
#if USE_SDFAT
	uint32_t writeHeader(File* file);
	void writeBuffer(uint16_t* buffer, uint16_t transparentColor, File* file);
	void writeFrame(uint16_t frame, uint16_t* buffer, uint16_t transparentColor, File* file);
#endif
	uint8_t depth;
	bool upside_down;
private:
	bool valid;
	uint8_t image_offset;
	uint32_t creatorBits;
	uint16_t width;
	uint16_t height;
	uint16_t frames;
	uint8_t indexMap[16];
};

} // namespace Gamebuino_Meta

#endif /// _GAMEBUINO_BMP_H_
