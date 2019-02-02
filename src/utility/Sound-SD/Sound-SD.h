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

#ifndef _GAMEBUINO_META_SOUND_SD_H_
#define _GAMEBUINO_META_SOUND_SD_H_

#include "../../config/config.h"
#include "../Sound.h"
#if USE_SDFAT
#include "../SdFat.h"
#endif

namespace Gamebuino_Meta {

class Sound_Handler_Wav : public Sound_Handler {
public:
	Sound_Handler_Wav(Sound_Channel* chan);
	~Sound_Handler_Wav();
	bool init(const char* filename);
	void update();
	void rewind();
	uint32_t getPos();
private:
#if USE_SDFAT
	File file;
#endif
	uint16_t head_index;
	uint32_t wav_offset;
	bool rewind_flag;
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_SOUND_SD_H_
