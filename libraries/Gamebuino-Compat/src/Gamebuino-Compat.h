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

#ifndef _GAMEBUINO_COMPAT_H_
#define _GAMEBUINO_COMPAT_H_


#include <Gamebuino-Meta.h>

#include "utility/Display_Compat.h"
#include "utility/Battery_Compat.h"
#include "utility/Sound_Compat.h"


namespace Gamebuino_Compat {

class Gamebuino : public Gamebuino_Meta::Gamebuino {
public:
	void begin();
	Battery battery;
	void getDefaultName(char* string);
	bool update();
	void titleScreen(const char* name, const uint8_t* logo = 0);
	void titleScreen(const uint8_t* logo);
};

} // namespace Gamebuino_Compat


const uint8_t NUM_BTN = 7;
const Button BTN_DOWN = BUTTON_DOWN;
const Button BTN_LEFT = BUTTON_LEFT;
const Button BTN_RIGHT = BUTTON_RIGHT;
const Button BTN_UP = BUTTON_UP;
const Button BTN_A = BUTTON_A;
const Button BTN_B = BUTTON_B;
const Button BTN_C = BUTTON_C;
using Gamebuino_Compat::Gamebuino;

extern Gamebuino_Compat::Gamebuino gb;


#define LCDHEIGHT gb.display.height()
#define LCDWIDTH gb.display.width()

#include <Gamebuino-EEPROM.h>

#endif // _GAMEBUINO_COMPAT_H_
