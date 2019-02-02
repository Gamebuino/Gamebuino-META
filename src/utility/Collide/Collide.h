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
 - Julien Giovinazzo
*/
#ifndef _GAMEBUINO_META_COLLIDE_H_
#define _GAMEBUINO_META_COLLIDE_H_

#include "../../config/config.h"

namespace Gamebuino_Meta {

class Collide {
	public:
		bool rectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1, int16_t x2, int16_t y2, int16_t w2, int16_t h2);  // Returns TRUE if the 2 rects overlap
		bool pointRect(int16_t pointX, int16_t pointY, uint16_t rectX, uint16_t rectY, uint16_t rectW, uint16_t rectH);  // Returns TRUE if the point overlaps the rect
		bool circleCircle(int16_t centerX1, int16_t centerY1, int16_t r1, int16_t centerX2, int16_t centerY2, int16_t r2);  // Returns TRUE if the 2 circles overlap
		bool pointCircle(int16_t pointX, int16_t pointY, int16_t centerX, int16_t centerY, int16_t r);  // Returns TRUE if the point overlaps the circle
};
};  // Namespace Gamebuino_Meta




#endif  // _GAMEBUINO_META_COLLIDE_H_
