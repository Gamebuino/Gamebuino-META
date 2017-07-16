/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
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

#include "Sound.h"

namespace Gamebuino_Meta {

void Sound::begin() {
	volumeMax = 1;
	//mute by default as tone() randomly crashed the console
	globalVolume = 0;
}

void Sound::playOK() {
	if (globalVolume) {
		tone(A0,1000,50);
	}
}

void Sound::playCancel() {
	if (globalVolume) {
		tone(A0,500,50);
	}
}

void Sound::playTick() {
	if (globalVolume) {
		tone(A0,1000,5);
	}
}

void Sound::setVolume(int8_t volume) {
	globalVolume = (volume < 0) ? volumeMax : volume % (volumeMax+1); //wrap volume value
}

uint8_t Sound::getVolume() {
	return globalVolume;
}

} // Gamebuino_Meta
