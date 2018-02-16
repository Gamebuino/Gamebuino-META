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

#ifndef _GAMEBUINO_META_SOUND_RAW_H_
#define _GAMEBUINO_META_SOUND_RAW_H_

#include "Sound.h"

namespace Gamebuino_Meta {

class Sound_Handler_Raw : public Sound_Handler {
public:
	Sound_Handler_Raw(Sound_Channel* chan, uint8_t* buffer, uint32_t lenght);
	void update();
	void rewind();
	uint32_t getPos();
private:
	uint8_t* buffer;
	uint32_t length;
};

}; // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_SOUND_RAW_H_
