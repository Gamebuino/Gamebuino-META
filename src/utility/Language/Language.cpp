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

#include "Language.h"

namespace Gamebuino_Meta {

LangCode Language::currentLang = LangCode::en;

LangCode Language::getCurrentLang() {
	return currentLang;
}

void Language::setCurrentLang(LangCode c) {
	currentLang = c;
}

const char* Language::get(const MultiLang* l, uint8_t num) {
	const char* def = 0;
	for (uint8_t i = 0; i < num; i++) {
		if (l[i].code == currentLang) {
			return l[i].str;
		}
		if (!def) {
			def = l[i].str;
		} else if (l[i].code == LangCode::en) {
			def = l[i].str;
		}
	}
	return def;
}

void Print_Language::print(const MultiLang* l, uint8_t num ) {
	print(Language::get(l, num));
}

void Print_Language::println(const MultiLang* l, uint8_t num ) {
	println(Language::get(l, num));
}

}; // Gamebuino_Meta
