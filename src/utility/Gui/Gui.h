/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2018

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
#ifndef _GAMEBUINO_META_GUI_H_
#define _GAMEBUINO_META_GUI_H_

#include "../../config/config.h"
#include "../Language.h"

namespace Gamebuino_Meta {

class Gui {
public:
// keyboard functions
	void keyboard(const char* title, char* text, uint8_t length);
#if LANGUAGE_DEFAULT_SIZE
	void keyboard(const MultiLang* title, char* text, uint8_t length, uint8_t numLang = LANGUAGE_DEFAULT_SIZE);
	template<uint8_t M>
	void keyboard(const MultiLang* title, char (&text) [M]) {
		keyboard(title, text, M-1, LANGUAGE_DEFAULT_SIZE);
	}
#else
	void keyboard(const MultiLang* title, char* text, uint8_t length, uint8_t numLang);
#endif
	template<uint8_t N>
	void keyboard(const MultiLang (&title) [N], char* text, uint8_t length) {
		keyboard(title, text, length, N);
	}
	template<uint8_t M>
	void keyboard(const char* title, char (&text) [M]) {
		keyboard(title, text, M-1);
	}
	template<uint8_t N, uint8_t M>
	void keyboard(const MultiLang (&title) [N], char (&text) [M]) {
		keyboard(title, text, M-1, N);
	}

// menu functions
	uint8_t menu(const char* title, const char** items, uint8_t length, uint8_t numLang = 0);
#if LANGUAGE_DEFAULT_SIZE
	uint8_t menu(const MultiLang* title, const MultiLang** items, uint8_t length, uint8_t numLang = LANGUAGE_DEFAULT_SIZE);
	template<uint8_t O>
	uint8_t menu(const MultiLang* title, const MultiLang* (&items) [O]) {
		return menu(title, items, O);
	}
#else
	uint8_t menu(const MultiLang* title, const MultiLang** items, uint8_t length, uint8_t numLang);
#endif
	template<uint8_t O, uint8_t N>
	uint8_t menu(const MultiLang (&title) [N], const MultiLang* (&items) [O]) {
		return menu(title, items, O, N);
	}
	template<uint8_t O>
	uint8_t menu(const char* title, const char* (&items) [O]) {
		return menu(title, items, O);
	}

// popup functions
#if GUI_ENABLE_POPUP
	void popup(const char* text, uint8_t duration);
#if LANGUAGE_DEFAULT_SIZE
	void popup(const MultiLang* text, uint8_t duration, uint8_t numLang = LANGUAGE_DEFAULT_SIZE);
#else
	void popup(const MultiLang* text, uint8_t duration, uint8_t numLang);
#endif
	template<uint8_t N>
	void popup(const MultiLang (&text) [N], uint8_t duration) {
		popup(text, duration, N);
	}
	void updatePopup();
#endif // GUI_ENABLE_POPUP
};

}; // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_GUI_H_
