/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *é
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef _GAMEBUINO_META_SOUND_H_
#define	_GAMEBUINO_META_SOUND_H_

#include <Arduino.h>

namespace Gamebuino_Meta {

class Sound {
public:
	void begin();
	void playOK();
	void playCancel();
	void playTick();
	
	void setVolume(int8_t volume);
	uint8_t getVolume();
	
	uint8_t volumeMax;
private:
	uint8_t globalVolume;
};

} // namespace Gamebuino_Meta

#endif	// _GAMEBUINO_META_SOUND_H_
