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

#ifndef _GAMEBUINO_META_SAVE_H_
#define _GAMEBUINO_META_SAVE_H_

#include "../../config/config.h"
#include "../Display-ST7735.h"

#if USE_SDFAT
#include "../SdFat.h"
extern SdFat SD;
#endif

#include <type_traits>


namespace Gamebuino_Meta {

struct SaveVar {
	bool defined;
	uint8_t type;
};

struct SaveDefault {
	const uint16_t i;
	const uint8_t type;
	const union {
		const int32_t ival;
		const void *ptr;
	} val;
	const uint32_t length;
//	SaveDefault(const uint8_t i, const uint8_t type, const int32_t ival): i(i), type(type), val{.ival=ival}, length(0){};
//	SaveDefault(const uint8_t i, const uint8_t type, const void* ptr, const uint8_t length): i(i), type(type), val{.ptr=ptr}, length(length){};
};

class Save {
public:
	Save(){};
	Save(const char* _savefile, const char* _checkbytes);
	void config(const SaveDefault* _defaults, uint16_t _num_defaults);
	void config(uint16_t _blocks, const SaveDefault* _defaults, uint16_t _num_defaults);
	template<uint16_t N>
	void config(const SaveDefault (&_defaults)[N]) {
		config(_defaults, N);
	}
	template<uint16_t N>
	void config(uint16_t _blocks, const SaveDefault (&_defaults)[N]) {
		config(_blocks, _defaults, N);
	}
	
	int32_t get(uint16_t i);
	bool get(uint16_t i, void* buf, uint32_t bufsize);
	template< typename T >
	bool get(uint16_t i, T& obj) {
		return get(i, &obj, sizeof(T));
	};
	bool set(uint16_t i, int32_t num);
	bool set(uint16_t i, char* buf);
	bool set(uint16_t i, const char* buf);
	bool set(uint16_t i, void* buf, uint32_t bufsize);
	bool set(uint16_t i, const void* buf, uint32_t bufsize);
	template<
		typename T,
		typename = typename std::enable_if<!(std::is_arithmetic<T>::value), T>::type
	>
	bool set(uint16_t i, T& obj) {
		return set(i, &obj, sizeof(T));
	};
	void del(uint16_t i);
private:
#if USE_SDFAT
	File f;
#endif
	bool open = false;
	bool readOnly = false;
	const char* checkbytes;
	const char* savefile;
	uint16_t num_defaults;
	uint16_t blocks;
	uint32_t payload_size = 0;
	const SaveDefault* defaults;
	void _set(uint16_t i, uint32_t b);
	uint32_t _get(uint16_t i);
	void newBlob(uint16_t i, uint32_t size);
	void openFile();
	void error(const char *s);
	SaveVar getVarInfo(uint16_t i);
};

} // namsepace Gamebuino_Meta

using Gamebuino_Meta::SaveVar;
using Gamebuino_Meta::SaveDefault;

#endif // _GAMEBUINO_META_SAVE_H_
