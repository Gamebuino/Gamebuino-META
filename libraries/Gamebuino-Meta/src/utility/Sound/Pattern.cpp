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

#include "Pattern.h"

namespace Gamebuino_Meta {

const uint8_t _halfPeriods[] = {95,89,84,80,75,71,67,63,60,56,53,50,47,45,42,40,38,35,33,32,30,28,27,25,24,22,21,20,19,18,17,16,15,14,13,13,12,11,11,10,9,9,8,8,7,7,7,6,6,6,5,5,5,4,4,4,4,4,3,3,3,3,3,2,2,2};

Sound_Handler_Pattern::Sound_Handler_Pattern(Sound_Channel* chan, uint8_t* _buffer) : Sound_Handler(chan) {
	channel->buffer = _buffer;
	channel->type = Sound_Channel_Type::square;
	channel->amplitude = 0x30;
	rewind();
	channel->use = true;
}

void Sound_Handler_Pattern::rewind() {
	note_duration = 0;
	patternCursor = 0;
}

void Sound_Handler_Pattern::update() {
	if (note_duration--) {
		return;
	}
	uint16_t data = ((uint16_t*)channel->buffer)[patternCursor++];
	if (!data) { // end of pattern - what to do?
		if (channel->loop) {
			rewind(); 
		} else {
			channel->use = false;
		}
		return;
	}
	while (data & 0x0001) { // read all the commands
		/*
		data >>= 2;
		uint8_t cmd = data & 0x0F;
		data >>= 4;
		uint8_t X = data & 0x1F;
		data >>= 5;
		int8_t Y = data - 16;
		*/
		// trash for now
		
		data = ((uint16_t*)channel->buffer)[patternCursor++];
	}
	data >>= 2;
	
	uint8_t id = data & 0x003F;
	if (id == 63) {
		channel->amplitude = 0;
	} else {
		channel->total = _halfPeriods[id] * 2;
		channel->amplitude = 0x30;
	}
	channel->index = 0;
	data >>= 6;
	note_duration = data;
}


}; // namespace Gamebuino_Meta
