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

#ifndef _GAMEBUINO_COMPAT_BATTERY_H_
#define _GAMEBUINO_COMPAT_BATTERY_H_

#include <Arduino.h>


namespace Gamebuino_Compat {

class Battery {
public:
	void begin();
	void update();
	bool show;
	uint8_t  level;
	uint16_t voltage;
	uint16_t thresholds[4];
	uint8_t  nextUpdate;
};

}; // namespace Gamebuino_Compat


#endif // _GAMEBUINO_COMPAT_BATTERY_H_
