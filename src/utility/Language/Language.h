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

#ifndef _GAMEBUINO_META_LANGUAGE_H_
#define _GAMEBUINO_META_LANGUAGE_H_

#include "../../config/config.h"
#include "LangCode.h"

#if NO_ARDUINO
#include <Print-Compat.h>
#else
#include <Print.h>
#endif

namespace Gamebuino_Meta {

struct MultiLang {
	const LangCode code;
	const char* str;
};

class Language {
private:
	static LangCode currentLang;
public:
	static LangCode getCurrentLang();
	static void setCurrentLang(LangCode c);
#if LANGUAGE_DEFAULT_SIZE
	static const char* get(const MultiLang* l, uint8_t num = LANGUAGE_DEFAULT_SIZE);
#else
	static const char* get(const MultiLang* l, uint8_t num);
#endif
	template<uint8_t N>
	static const char* get(const MultiLang (&l) [N]) {
		return get(l, N);
	};
	template<uint8_t N>
	static const char* _get(const MultiLang (&l) [N]) {
		return get(l, N);
	};
};



class Print_Language : public Print {
public:
	using Print::print;
	using Print::println;
#if USE_PRINTF
	using Print::printf;
#endif // USE_PRINTF
#if LANGUAGE_DEFAULT_SIZE
	void print(const MultiLang* l, uint8_t num = LANGUAGE_DEFAULT_SIZE);
	void println(const MultiLang* l, uint8_t num = LANGUAGE_DEFAULT_SIZE);
#else
	void print(const MultiLang* l, uint8_t num);
	void println(const MultiLang* l, uint8_t num);
#endif
	template <uint8_t N>
	void print(const MultiLang (&l) [N]) {
		print(l, N);
	};
	template <uint8_t N>
	void println(const MultiLang (&l) [N]) {
		println(l, N);
	};
#if USE_PRINTF
	template <uint8_t N>
	void printf(const MultiLang (&l) [N], ...) {
		char buf[PRINTF_BUF];
		va_list ap;
		const char* format = Language::get(l, N);
		va_start(ap, format);
		vsnprintf(buf, sizeof(buf), format, ap);
		write(buf);
		va_end(ap);
	};
#endif // USE_PRINTF
};


}; // namespace Gamebuino_Meta

using Gamebuino_Meta::MultiLang;

#endif // _GAMEBUINO_META_LANGUAGE_H_
