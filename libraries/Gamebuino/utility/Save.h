/*
 * (C) Copyright 2017 Sorunome. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef SAVE_H
#define SAVE_H

#include "../config.h"
#include <SdFat.h>
#include <Adafruit_ST7735.h>
extern SdFat SD;


struct SaveVar {
	bool defined;
	uint8_t type;
};

struct SaveDefault {
	SaveDefault(uint16_t _i, uint8_t _type, uint8_t _ival);
	SaveDefault(uint16_t i, uint8_t _type, const void* _ptr, uint8_t _length);
	uint16_t i;
	uint8_t type;
	union {
		uint32_t ival;
		const void *ptr;
	} val;
	uint8_t length;
};

class Save {
public:
	Save(Adafruit_ST7735 *_tft);
	void set(uint16_t i, int32_t num);
	int32_t get(uint16_t i);
	bool get(uint16_t i, void* buf, uint8_t bufsize);
	void set(uint16_t i, char* buf);
	void set(uint16_t i, const char* buf);
	void set(uint16_t i, void* buf, uint8_t bufsize);
	void set(uint16_t i, const void* buf, uint8_t bufsize);
	void del(uint16_t i);
private:
	Adafruit_ST7735 *tft;
	const char *filename = "save.sav";
	File f;
	bool exists = false;
	bool open = false;
	uint16_t blocks = 0;
	uint32_t payload_size = 0;
	SaveDefault defaults[SAVECONF_SIZE] = SAVECONF;
	void _set(uint16_t i, uint32_t b);
	uint32_t _get(uint16_t i);
	void newBlob(uint16_t i, uint8_t size);
	void openFile();
	void error(const char *s);
	SaveVar getVarInfo(uint16_t i);
};


#endif // SAVE_H
