/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Valden 2018

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
- Valden
*/

#include "Sound_FX.h"

namespace Gamebuino_Meta {
void Sound_Handler_FX::init() {
	_current_Sound_FX_volume = 0;
	_current_Sound_FX_period = 0;
	_noise_period = 0;

	_current_pattern_Sound_FX = UINT8_MAX;
	_current_pattern_length = 0;
	_current_pattern = NULL;

	_pitch_scale = 1 << FPP;

	resetGenerators();
}

void Sound_Handler_FX::update() {
	// Check if we should advance in the pattern
	if (_current_Sound_FX_time != UINT32_MAX && _current_Sound_FX_time >= _current_Sound_FX_length) {
		// Check if there is still fx to play
		if ((_current_pattern_length != 0 && _current_pattern_Sound_FX < _current_pattern_length - 1) 
			|| (_current_pattern_length == 0 && (_current_pattern[_current_pattern_Sound_FX].continue_flag))) {
			_current_pattern_Sound_FX++;
			play(_current_pattern[_current_pattern_Sound_FX]);
		}
	}


	// Generate sound
	if (_current_Sound_FX_time < _current_Sound_FX_length) {
		switch (_current_Sound_FX.type) {
		case Sound_FX_Wave::NOISE:
			generateNoise();
			break;
		case Sound_FX_Wave::SQUARE:
			generateSquare();
			break;
		default:
			// WTF man
			break;
		}
	}
	else if (_current_Sound_FX_time != UINT32_MAX) {
		memset(parent_channel->buffer, 0, parent_channel->size);
		_current_Sound_FX_time = UINT32_MAX;
	}
	else{
		//do nothing
	}
}

void Sound_Handler_FX::play(const Sound_FX & Sound_FX) {
	_current_Sound_FX = Sound_FX;
	_current_Sound_FX_time = 0;
	_current_Sound_FX_volume = Sound_FX.volume_start << VOLUME_START_SCALE;
	_current_Sound_FX_volume_sweep = (Sound_FX.volume_sweep << VOLUME_SWEEP_SCALE) * SR_DIVIDER;
	_current_Sound_FX_period = Sound_FX.period_start << PERIOD_START_SCALE;
	_current_Sound_FX_period_sweep = (Sound_FX.period_sweep << PERIOD_SWEEP_SCALE) * SR_DIVIDER;
	_current_Sound_FX_length = Sound_FX.length * LENGTH_SCALE;
	resetGenerators();
}

void Sound_Handler_FX::play(const Sound_FX * const pattern, uint8_t length) {
	_current_pattern = pattern;
	_current_pattern_Sound_FX = 0;
	_current_pattern_length = length;
	play(_current_pattern[_current_pattern_Sound_FX]);
}

void Sound_Handler_FX::play(const Sound_FX * const pattern, uint8_t length, uint16_t pitch_scale) {
	_current_pattern = pattern;
	_current_pattern_Sound_FX = 0;
	_current_pattern_length = length;
	play(_current_pattern[_current_pattern_Sound_FX]);
	_pitch_scale = pitch_scale;
}

void Sound_Handler_FX::resetGenerators() {
	_square_period = 0;
	_noise_period = 0;
	_square_polarity = 1;
}

void Sound_Handler_FX::generateNoise() {
	uint32_t target = parent_channel->index;
	static uint16_t lfsr = 1;
	do{
		int8_t volume = getVolume();

		if (--_noise_period <= 0) {
			_noise_period = max((getFrequency() >> (FPP + 4)) / SR_DIVIDER, 0);
			bool bit = (lfsr ^ (lfsr >> 1)) & 1;
			lfsr = (lfsr >> 1) ^ (bit << 14);
		}
		volume = (lfsr & 1) ?  getVolume() :  -getVolume();
		uint8_t v = (uint8_t)volume;
		uint32_t sample = (v << 24) | (v << 16) | (v << 8) | v;

		_head_index = (_head_index + 1) % (parent_channel->size >> 2);
		_current_Sound_FX_time += 4 * SR_DIVIDER;
		((uint32_t*)(&parent_channel->buffer[0]))[_head_index] = sample;
	} while (_head_index != target >> 2);
}

void Sound_Handler_FX::generateSquare() {
	uint32_t target = parent_channel->index;
	uint32_t * buffer_ptr = ((uint32_t*)(&parent_channel->buffer[0]));

	do {
		uint32_t sample = 0;
		int8_t volume = getVolume();
		if (_square_period <= (4 << FPP))
		{
			for (int i = 0; i < 4; i++)
			{
				_square_period -= (1 << FPP);
				if (_square_period <= 0) {
					_square_period += max(getFrequency() / SR_DIVIDER, 0);
					_square_polarity = !_square_polarity;
				}
				int8_t vol = _square_polarity ? volume : -volume;
				uint8_t v = (uint8_t)vol;
				sample |= (v << (8 * i));
			}
		}
		else
		{
			_square_period -= (4 << FPP);
			volume = _square_polarity ? volume : -volume;
			uint8_t v = (uint8_t)volume;
			sample = (v << 24) | (v << 16) | (v << 8) | v;
		}

		_head_index = (_head_index + 1) % (parent_channel->size >> 2);
		_current_Sound_FX_time += 4 * SR_DIVIDER;
		buffer_ptr[_head_index] = sample;
	} while (_head_index != target >> 2);

}



} // Namespace Gamebuino_META

