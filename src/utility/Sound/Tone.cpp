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

#include "Tone.h"
#include "../../Gamebuino-Meta.h"

namespace Gamebuino_Meta {

extern Gamebuino* gbptr;

extern int8_t tone_identifier;

Sound_Handler_Tone::Sound_Handler_Tone(Sound_Channel* chan, uint32_t frequency, int32_t duration, int8_t i) : Sound_Handler(chan) {
	channel->type = Sound_Channel_Type::square;
	channel->amplitude = 0x30;
	channel->total = 22050 / frequency;
	channel->index = 0;
	channel->buffer = (uint8_t*)duration; // that's what i call abusing
	identifier = i;
	channel->use = true;
}

Sound_Handler_Tone::~Sound_Handler_Tone() {
	if (tone_identifier == identifier) {
		tone_identifier = -1;
	}
}

void Sound_Handler_Tone::update() {
	if (channel->loop) {
		return;
	}
	int32_t duration = (int32_t)channel->buffer;
	duration -= gbptr->getTimePerFrame();
	channel->buffer = (uint8_t*)duration;
	if (duration <= 0) {
		channel->use = false;
		if (tone_identifier == identifier) {
			tone_identifier = -1;
		}
	}
}

void Sound_Handler_Tone::rewind() {
	// trash
}

}; // namespace Gamebuino_Meta
