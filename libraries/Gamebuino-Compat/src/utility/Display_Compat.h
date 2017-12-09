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

#ifndef _GAMEBUINO_COMPAT_DISPLAY_H_
#define _GAMEBUINO_COMPAT_DISPLAY_H_

// include Image.h from Gamebuino-Meta
#include <utility/Image.h>

namespace Gamebuino_Compat {

class Display : public Gamebuino_Meta::Image {
public:
	Display();
	void clear();
	void fillScreen();
	void fillScreen(Color color);
	void fillScreen(ColorIndex color);
	bool persistence = false;
};

}; // namespace Gamebuino_Compat

using Gamebuino_Compat::Display;

#endif // _GAMEBUINO_COMPAT_DISPLAY_H_
