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

#include "Collide.h"

namespace Gamebuino_Meta {
	bool Collide::rectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1, int16_t x2, int16_t y2, int16_t w2, int16_t h2) {
		return (x2 < x1 + w1 && x2 + w2 > x1 
			&& y2 < y1 + h1 && y2 + h2 > y1);
	}
	
	bool Collide::pointRect(int16_t pointX, int16_t pointY, uint16_t rectX, uint16_t rectY, uint16_t rectW, uint16_t rectH) {
		return (pointX >= rectX && pointX < rectX + rectW && pointY >= rectY && pointY < rectY + rectH);
	}
	bool Collide::circleCircle(int16_t centerX1, int16_t centerY1, int16_t r1, int16_t centerX2, int16_t centerY2, int16_t r2) {
		// True if the distance between the centers is smaller than the sum of the radii : dist < r1 + r2
		// dist = sqrt( (centerX1 - centerX2)^2 + (centerY1 - centerY2)^2 ) but we can't use sqrt (too slow).
		// however, sqrt(a) < b  <==>  a < b*b  for all natural numbers
		// in our case, 'a' is by definition a natural numbers
		// So let's make sure 'b' is a natural numbers
		r1 = abs(r1);
		r2 = abs(r2);
		// Now apply the formula
		return (((centerX1 - centerX2) * (centerX1 - centerX2) + (centerY1 - centerY2) * (centerY1 - centerY2)) < (r1 + r2) * (r1 + r2));
	}
	bool Collide::pointCircle(int16_t pointX, int16_t pointY, int16_t centerX, int16_t centerY, int16_t r) {
		// Just like circleCircle collision, but r2 = 0
		r = abs(r);
		return (((pointX - centerX) * (pointX - centerX) + (pointY - centerY) * (pointY - centerY)) < r * r);
	}
		

};  // Namespace Gamebuino_Meta
