/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2014, 2016-2017

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
 - Aurelien Rodot
 - Sorunome
*/

#ifndef _GAMEBUINO_META_BUTTONS_H_
#define	_GAMEBUINO_META_BUTTONS_H_

#include "../../config/config.h"

namespace Gamebuino_Meta {
const uint8_t NUM_BTN = 8;
const uint8_t BTN_CS = 25;

enum class Button : uint8_t {
	down = 0,
	left,
	right,
	up,
	a,
	b,
	menu,
	home,
};

class Buttons {
public:
	void begin();
	void update();
	bool pressed(Button button);
	bool released(Button button);
	bool held(Button button, uint16_t time);
	bool repeat(Button button, uint16_t period);
	uint16_t timeHeld(Button button);
	uint16_t states[NUM_BTN];
};

} // namespace Gamebuino_Meta

using Gamebuino_Meta::Button;

const Button BUTTON_DOWN = Button::down;
const Button BUTTON_LEFT = Button::left;
const Button BUTTON_RIGHT = Button::right;
const Button BUTTON_UP = Button::up;
const Button BUTTON_A = Button::a;
const Button BUTTON_B = Button::b;
const Button BUTTON_MENU = Button::menu;
const Button BUTTON_HOME = Button::home;
const Button BUTTON_C = Button::menu;
const Button BUTTON_D = Button::home;

#endif	// _GAMEBUINO_META_BUTTONS_H_
