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
	_current_pattern = nullptr;

	_pitch_scale = 1 << FPP;

	_headP = (uint32_t *)parent_channel->buffer;
	_firstZeroP = nullptr;

	resetGenerators();
}

void Sound_Handler_FX::update() {
	// Set target to completely fill buffer (given current index, which is continuously changing)
	// Note: Casting before adding offset to ensure index is properly rounded down.
	uint32_t* targetHeadP = ((uint32_t *)parent_channel->buffer) + (parent_channel->index >> 2);

	uint32_t* maxHeadP = ((uint32_t *)parent_channel->buffer) + (parent_channel->size >> 2);

	// Fill buffer in one or more iterations. Multiple iterations are needed when the end of the
	// cyclic buffer is reached, or the active sound pattern is finished
	while (_headP != targetHeadP) {
		// Check if we should advance in the pattern
		if (_current_Sound_FX_time != UINT32_MAX && _current_Sound_FX_time >= _current_Sound_FX_length) {
			// Check if there is still fx to play
			if ((
				_current_pattern_length != 0 &&
				_current_pattern_Sound_FX < _current_pattern_length - 1
			) || (
				_current_pattern_length == 0 &&
				_current_pattern != nullptr &&
				_current_pattern[_current_pattern_Sound_FX].continue_flag
			)) {
				_current_pattern_Sound_FX++;
				play(_current_pattern[_current_pattern_Sound_FX]);
			} else {
				_current_Sound_FX_time = UINT32_MAX;
			}
		}

		// Generate sound

		// The maximum number of samples that can be filled without passing the target mark or the
		// end of the buffer cyclic buffer. Checking this here is more efficient than doing so in
		// inner loops that generate the actually sound samples.
		uint16_t maxSamples;
		if (_headP < targetHeadP) {
			maxSamples = targetHeadP - _headP;
		} else {
			maxSamples = maxHeadP - _headP;
		}

		if (_current_Sound_FX_time < _current_Sound_FX_length) {
			// Ensure at least one sample is added, to ensure termination
			maxSamples = max(1, min(
				maxSamples,
				((_current_Sound_FX_length - _current_Sound_FX_time) >> 2) / SR_DIVIDER
			));
			_current_Sound_FX_time += 4 * SR_DIVIDER * maxSamples;

			switch (_current_Sound_FX.type) {
				case Sound_FX_Wave::NOISE:
					generateNoise(_headP + maxSamples);
					break;
				case Sound_FX_Wave::SQUARE:
					generateSquare(_headP + maxSamples);
					break;
				default:
					// WTF man
					break;
			}
		} else { // Silence
			if (_firstZeroP == nullptr) {
				// Remember where first zero is written
				_firstZeroP = _headP;
			} else if (_firstZeroP == _headP) {
				// Buffer contains only zeroes. Nothing needs doing anymore
				return;
			} else if (_firstZeroP > _headP) {
				// Stop once buffer contains only zeroes
				maxSamples = min(maxSamples, _firstZeroP - _headP);
			}

			generateSilence(_headP + maxSamples);
		}

		if (_headP == maxHeadP) {
			// Reached end of cyclic buffer. Continue at the beginning.
			_headP = (uint32_t *)parent_channel->buffer;
		}
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
	_firstZeroP = nullptr;
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

void Sound_Handler_FX::generateNoise(uint32_t* endP) {
	static uint16_t lfsr = 1;
	do {
		int8_t volume = getVolume();

		if (--_noise_period <= 0) {
			_noise_period = max((getFrequency() >> (FPP + 4)) / SR_DIVIDER, 0);
			bool bit = (lfsr ^ (lfsr >> 1)) & 1;
			lfsr = (lfsr >> 1) ^ (bit << 14);
		}
		volume = (lfsr & 1) ?  getVolume() :  -getVolume();
		uint8_t v = (uint8_t)volume;
		uint32_t sample = (v << 24) | (v << 16) | (v << 8) | v;

		*_headP++ = sample;
	} while (_headP != endP);
}

void Sound_Handler_FX::generateSquare(uint32_t* endP) {
	do {
		uint32_t sample = 0;
		int8_t volume = getVolume();
		if (_square_period <= (4 << FPP)) {
			for (int i = 0; i < 4; i++) {
				_square_period -= (1 << FPP);
				if (_square_period <= 0) {
					_square_period += max(getFrequency() / SR_DIVIDER, 0);
					_square_polarity = !_square_polarity;
				}
				int8_t vol = _square_polarity ? volume : -volume;
				uint8_t v = (uint8_t)vol;
				sample |= (v << (8 * i));
			}
		} else {
			_square_period -= (4 << FPP);
			volume = _square_polarity ? volume : -volume;
			uint8_t v = (uint8_t)volume;
			sample = (v << 24) | (v << 16) | (v << 8) | v;
		}

		*_headP++ = sample;
	} while (_headP != endP);
}

void Sound_Handler_FX::generateSilence(uint32_t* endP) {
	do {
		*_headP++ = 0;
	} while (_headP != endP);
}

} // Namespace Gamebuino_META
