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

const uint8_t _halfPeriods[] = {
	95*SOUND_FREQ/44100,
	89*SOUND_FREQ/44100,
	84*SOUND_FREQ/44100,
	80*SOUND_FREQ/44100,
	75*SOUND_FREQ/44100,
	71*SOUND_FREQ/44100,
	67*SOUND_FREQ/44100,
	63*SOUND_FREQ/44100,
	60*SOUND_FREQ/44100,
	56*SOUND_FREQ/44100,
	53*SOUND_FREQ/44100,
	50*SOUND_FREQ/44100,
	47*SOUND_FREQ/44100,
	45*SOUND_FREQ/44100,
	42*SOUND_FREQ/44100,
	40*SOUND_FREQ/44100,
	38*SOUND_FREQ/44100,
	35*SOUND_FREQ/44100,
	33*SOUND_FREQ/44100,
	32*SOUND_FREQ/44100,
	30*SOUND_FREQ/44100,
	28*SOUND_FREQ/44100,
	27*SOUND_FREQ/44100,
	25*SOUND_FREQ/44100,
	24*SOUND_FREQ/44100,
	22*SOUND_FREQ/44100,
	21*SOUND_FREQ/44100,
	20*SOUND_FREQ/44100,
	19*SOUND_FREQ/44100,
	18*SOUND_FREQ/44100,
	17*SOUND_FREQ/44100,
	16*SOUND_FREQ/44100,
	15*SOUND_FREQ/44100,
	14*SOUND_FREQ/44100,
	13*SOUND_FREQ/44100,
	13*SOUND_FREQ/44100,
	12*SOUND_FREQ/44100,
	11*SOUND_FREQ/44100,
	11*SOUND_FREQ/44100,
	10*SOUND_FREQ/44100,
	9*SOUND_FREQ/44100,
	9*SOUND_FREQ/44100,
	8*SOUND_FREQ/44100,
	8*SOUND_FREQ/44100,
	7*SOUND_FREQ/44100,
	7*SOUND_FREQ/44100,
	7*SOUND_FREQ/44100,
	6*SOUND_FREQ/44100,
	6*SOUND_FREQ/44100,
	6*SOUND_FREQ/44100,
	5*SOUND_FREQ/44100,
	5*SOUND_FREQ/44100,
	5*SOUND_FREQ/44100,
	4*SOUND_FREQ/44100,
	4*SOUND_FREQ/44100,
	4*SOUND_FREQ/44100,
	4*SOUND_FREQ/44100,
	4*SOUND_FREQ/44100,
	3*SOUND_FREQ/44100,
	3*SOUND_FREQ/44100,
	3*SOUND_FREQ/44100,
	3*SOUND_FREQ/44100,
	3*SOUND_FREQ/44100,
	2*SOUND_FREQ/44100,
	2*SOUND_FREQ/44100,
	2*SOUND_FREQ/44100,
};

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

uint32_t Sound_Handler_Pattern::getPos() {
	return patternCursor;
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
