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

#include "Misc.h"

namespace Gamebuino_Meta {

// create a unique path thing
// call via etc.
// char name[] = "/TMP0000.BIN";
// sd_path_no_duplicate(name, 4, 4); // returns true on success, false on no success
void intToStr(int32_t i, char* buf) {
	while (true) {
		*(buf) = '0' + (i % 10);
		if (i < 10) {
			break;
		}
		i /= 10;
		buf--;
	}
}

int32_t sdPathNoDuplicate(char* name, uint8_t offset, uint8_t digits, int32_t start) {
#if USE_SDFAT
	int32_t power = 1;
	for (uint8_t j = 0; j < digits; j++) {
		power *= 10;
	}
	
	char* buf = name + offset + digits - 1;
	for (int32_t i = start; i < power; i++) {
		intToStr(i, buf);
		if (!SD.exists(name)) {
			return i; // we are done folks!
		}
	}
	return -1;
#else
	return -1;
#endif
}

uint16_t rgb888Torgb565(RGB888 c) {
	return ((c.r & 0xF8) << 8) | ((c.g & 0xFC) << 3) | (c.b >> 3);
}

RGB888 rgb565Torgb888(uint16_t c) {
	RGB888 out;
	
	out.r = (uint8_t)((c >> 8) & 0xF8);
	out.r |= out.r >> 5;
	
	out.g = (uint8_t)((c >> 3) & 0xFC);
	out.g |= out.g >> 6;
	
	out.b = (uint8_t)(c << 3);
	out.b |= out.b >> 5;
	return out;
}

#if USE_SDFAT
uint16_t f_read16(File* f) {
	uint16_t result;
	f->read(&result, 2);
	return result;
}

uint32_t f_read32(File* f) {
	uint32_t result;
	f->read(&result, 4);
	return result;
}

void f_write32(uint32_t b, File* f) {
	//Write four bytes
	//Luckily our MCU is little endian so byte order like this is fine
	f->write(&b, 4);
}

void f_write16(uint16_t b, File* f) {
	//Write two bytes
	//Luckily our MCU is little endian so byte order like this is fine
	f->write(&b, 2);
}
#endif // USE_SDFAT


} // namespace Gamebuino_Meta
