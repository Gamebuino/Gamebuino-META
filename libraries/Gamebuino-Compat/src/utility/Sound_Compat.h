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

#ifndef _GAMEBUINO_COMPAT_SOUND_H_
#define _GAMEBUINO_COMPAT_SOUND_H_

#include <Arduino.h>
// Grab the gamebuino-meta config
#include <config/config.h>
#include <utility/Sound.h>

namespace Gamebuino_Compat {

class Sound : public Gamebuino_Meta::Sound {
public:
	void begin();
	
	void playTrack(const unsigned int* track, uint8_t channel);
	void playTrack(const uint16_t* track, uint8_t channel);
	void updateTrack(uint8_t channel);
	void updateTrack();
	void stopTrack(uint8_t channel);
	void stopTrack();
	void changePatternSet(const unsigned int* const* patterns, uint8_t channel);
	void changePatternSet(const uint16_t* const* patterns, uint8_t channel);
	bool trackIsPlaying[SOUND_CHANNELS];
	
	void playPattern(const unsigned int* pattern, uint8_t channel);
	void playPattern(const uint16_t* pattern, uint8_t channel);
	void changeInstrumentSet(const unsigned int* const* instruments, uint8_t channel);
	void changeInstrumentSet(const uint16_t* const* instruments, uint8_t channel);
	void updatePattern(uint8_t i);
	void updatePattern();
	void setPatternLooping(boolean loop, uint8_t channel);
	void stopPattern(uint8_t channel);
	void stopPattern();
	bool patternIsPlaying[SOUND_CHANNELS];
	
	void command(uint8_t cmd, uint8_t X, int8_t Y, uint8_t i);
	void playNote(uint8_t pitch, uint8_t duration, uint8_t channel);
	void updateNote();
	void updateNote(uint8_t i);
	void stopNote(uint8_t channel);
	void stopNote();
	
	uint8_t outputPitch[SOUND_CHANNELS];
	int8_t outputVolume[SOUND_CHANNELS];

	void setVolume(int8_t volume);
	uint8_t getVolume();
	void setVolume(int8_t volume, uint8_t channel);
	uint8_t getVolume(uint8_t channel);
	
	uint8_t prescaler;

	void setChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod);

	uint8_t globalVolume;
	uint8_t volumeMax;
};

}; // namespace Gamebuino_Compat

#endif // _GAMEBUINO_COMPAT_SOUND_H_
