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

#include "Sound_Compat.h"

namespace Gamebuino_Compat {

void Sound::begin() {
	Gamebuino_Meta::Sound::begin();
#if SOUND_CHANNELS
	for (uint8_t i = 0; i < SOUND_CHANNELS; i++) {
		trackIsPlaying[i] = false;
		patternIsPlaying[i] = false;
		outputPitch[i] = 0;
		outputVolume[i] = 0;
	}
#endif
	prescaler = 0;
	globalVolume = 0;
	volumeMax = 0;
}

void Sound::setVolume(int8_t volume) {
	Gamebuino_Meta::Sound::setVolume(volume);
}

uint8_t Sound::getVolume() {
	return Gamebuino_Meta::Sound::getVolume();
}


void Sound::playTrack(const unsigned int* track, uint8_t channel) {
	// trash
}
void Sound::playTrack(const uint16_t* track, uint8_t channel) {
	// trash
}
void Sound::updateTrack(uint8_t channel) {
	// trash
}
void Sound::updateTrack() {
	// trash
}
void Sound::stopTrack(uint8_t channel) {
	// trash
}
void Sound::stopTrack() {
	// trash
}
void Sound::changePatternSet(const unsigned int* const* patterns, uint8_t channel) {
	// trash
}
void Sound::changePatternSet(const uint16_t* const* patterns, uint8_t channel) {
	// trash
}
void Sound::playPattern(const unsigned int* pattern, uint8_t channel) {
	// trash
}
void Sound::playPattern(const uint16_t* pattern, uint8_t channel) {
	// trash
}
void Sound::changeInstrumentSet(const unsigned int* const* instruments, uint8_t channel) {
	// trash
}
void Sound::changeInstrumentSet(const uint16_t* const* instruments, uint8_t channel) {
	// trash
}
void Sound::updatePattern(uint8_t i) {
	// trash
}
void Sound::updatePattern() {
	// trash
}
void Sound::setPatternLooping(boolean loop, uint8_t channel) {
	// trash
}
void Sound::stopPattern(uint8_t channel) {
	// trash
}
void Sound::stopPattern() {
	// trash
}
void Sound::command(uint8_t cmd, uint8_t X, int8_t Y, uint8_t i) {
	// trash
}
void Sound::playNote(uint8_t pitch, uint8_t duration, uint8_t channel) {
	// trash
}
void Sound::updateNote() {
	// trash
}
void Sound::updateNote(uint8_t i) {
	// trash
}
void Sound::stopNote(uint8_t channel) {
	// trash
}
void Sound::stopNote() {
	// trash
}
void Sound::setVolume(int8_t volume, uint8_t channel) {
	// trash
}
uint8_t Sound::getVolume(uint8_t channel) {
	return 0;
}
void Sound::setChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod) {
	// trash
}

}; // namespace Gamebuino_Compat
