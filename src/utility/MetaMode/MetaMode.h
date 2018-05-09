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
#ifndef _GAMEBUINO_META_METAMODE_H_
#define _GAMEBUINO_META_METAMODE_H_

#include <Arduino.h>
#include "../../config/config.h"
#include "../Language.h"

namespace Gamebuino_Meta {

class MetaMode {
   public:
	void update();
	bool isActive() {
		handled = true;
		return active;
	};

   private:
	bool handled = false;  // False by default. Becomes true the first time "isActive()" is called
	bool active = false;   // True once MetaMode has been activated. Cannot be turned off

	// Draws the moving loading lines at the top and bottom of the screen. The lines are horizontal. Should only be called once per frame
	// and centered 
	// Parameters :	uint8_t percentage	:	0 is not drawn, 100 takes up the whole screen
	void drawLoadingLines(uint8_t percentage);

	// Draw "META" text to screen
	void drawTextMeta(int8_t x, int8_t y);
	// Draw "MODE" text to screen
	void drawTextMode(int8_t x, int8_t y);
};

};  // Namespace Gamebuino_Meta

#endif  // _GAMEBUINO_META_METAMODE_H_