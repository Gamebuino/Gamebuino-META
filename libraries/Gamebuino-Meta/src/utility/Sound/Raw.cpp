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

#include "Raw.h"

namespace Gamebuino_Meta {

Sound_Handler_Raw::Sound_Handler_Raw(Sound_Channel* chan, uint8_t* _buffer, uint32_t _length) : Sound_Handler(chan) {
	buffer = _buffer;
	length = _length;
	rewind();
};

void Sound_Handler_Raw::update() {
	channel->buffer += channel->index;
	channel->index = 0;
	
	uint32_t cursor = (uint32_t)channel->buffer - (uint32_t)buffer;
	if (cursor + 0xFFFF <= length) {
		channel->total = length - cursor;
		channel->last = true;
	}
}

void Sound_Handler_Raw::rewind() {
	channel->index = 0;
	channel->total = length > 0xFFFF ? 0xFFFF : length;
	channel->buffer = buffer;
	channel->type = Sound_Channel_Type::raw;
	
	channel->use = true;
}

uint32_t Sound_Handler_Raw::getPos() {
	return channel->index;
}

}; // namespace Gamebuino_Meta
