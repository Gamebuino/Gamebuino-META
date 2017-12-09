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

#include "Display_Compat.h"

namespace Gamebuino_Compat {

Display::Display() : Gamebuino_Meta::Image(80, 64, ColorMode::rgb565) {
	persistence = false;
}

void Display::clear() {
	Gamebuino_Meta::Image::fill(Color::white);
	setCursors(0, 0);
}

void Display::fillScreen() {
	fill();
}

void Display::fillScreen(Color color) {
	fill(color);
}

void Display::fillScreen(ColorIndex color) {
	fill(color);
}

}; // namespace Gamebuino_Compat
