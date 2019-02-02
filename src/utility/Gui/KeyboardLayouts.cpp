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

// This file is ISO-8859-1 encoded, or anything compatible, such as Windows-1252

#include "../../config/config.h"

namespace Gamebuino_Meta {

const char keyboardLayoutPage0[] = "1234567890qwertyuiop[]*asdfghjkl;'\\*zxcvbnm,./-=*AB";
const char keyboardLayoutPage1[] = "!@*$%^&*()QWERTYUIOP{}*ASDFGHJKL:\"|*ZXCVBNM<>?_+*ab";
const char* keyboardLayout[] = {
	keyboardLayoutPage0,
	keyboardLayoutPage1,
};
const uint8_t keyboardLayoutPages = 2;

}; // namespace Gamebuino_Meta
