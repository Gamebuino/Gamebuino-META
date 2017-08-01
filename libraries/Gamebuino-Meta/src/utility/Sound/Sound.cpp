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
#include "../Sound-SD.h"
#include "../../config/config.h"

namespace Gamebuino_Meta {

#if SOUND_CHANNELS > 0
Sound_Channel channels[SOUND_CHANNELS];
Sound_Handler* handlers[SOUND_CHANNELS];

bool tcIsSyncing() {
	return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

void tcStart() {
	// Enable TC

	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
	while (tcIsSyncing());
}

void tcReset() {
	// Reset TCx
	TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
	while (tcIsSyncing());
	while (TC5->COUNT16.CTRLA.bit.SWRST);
}

void tcDisable() {
	// Disable TC5
	TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	while (tcIsSyncing());
}

void tcConfigure(uint32_t sampleRate) {
	// Enable GCLK for TCC2 and TC5 (timer counter input clock)
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
	while (GCLK->STATUS.bit.SYNCBUSY);

	tcReset();

	// Set Timer counter Mode to 16 bits
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;

	// Set TC5 mode as match frequency
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;

	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;

	TC5->COUNT16.CC[0].reg = (uint16_t) (SystemCoreClock / sampleRate - 1);
	while (tcIsSyncing());
	
	// Configure interrupt request
	NVIC_DisableIRQ(TC5_IRQn);
	NVIC_ClearPendingIRQ(TC5_IRQn);
	NVIC_SetPriority(TC5_IRQn, 0);
	NVIC_EnableIRQ(TC5_IRQn);

	// Enable the TC5 interrupt request
	TC5->COUNT16.INTENSET.bit.MC0 = 1;
	while (tcIsSyncing());
}

void dacConfigure(void) {
	analogWriteResolution(10);
	analogWrite(A0, 0);
}

int8_t findEmptyChannel() {
	for (uint8_t i = 0; i < SOUND_CHANNELS; i++) {
		if (!channels[i].use) {
			if (handlers[i]) {
				delete handlers[i];
				handlers[i] = 0;
			}
			return i;
		}
	}
	return -1;
}
#endif

Sound_Handler::Sound_Handler(Sound_Channel* _channel) {
	channel = _channel;
}


void Sound::begin() {
	volumeMax = 1;
	//mute by default as tone() randomly crashed the console
	globalVolume = 0;
#if SOUND_CHANNELS > 0
	dacConfigure();
	tcConfigure(44100);
	tcStart();
#endif
}

int8_t Sound::play(const char* filename) {
#if SOUND_CHANNELS > 0
	int8_t i = findEmptyChannel();
	if (i < 0 || i >= SOUND_CHANNELS) {
		return -1; // no free channels atm
	}
	handlers[i] = new Sound_Handler_Wav(&(channels[i]));
	if (!((Sound_Handler_Wav*)handlers[i])->init(filename)) {
		delete handlers[i];
		handlers[i] = 0;
		return -1;
	}
	return i;
#endif // SOUND_CHANNELS
	return -1;
}

int8_t Sound::play(char* filename) {
	return play((const char*)filename);
}

void Sound::stop(int8_t i) {
	if (i >= SOUND_CHANNELS || i < 0) {
		return;
	}
	channels[i].use = false;
	if (handlers[i]) {
		delete handlers[i];
		handlers[i] = 0;
	}
}

void Sound::playOK() {
	if (globalVolume) {
//		tone(A0,1000,50);
	}
}

void Sound::playCancel() {
	if (globalVolume) {
//		tone(A0,500,50);
	}
}

void Sound::playTick() {
	if (globalVolume) {
//		tone(A0,1000,5);
	}
}

void Sound::update() {
#if SOUND_CHANNELS > 0
	for (uint8_t i = 0; i < SOUND_CHANNELS; i++) {
		if (channels[i].use) {
			handlers[i]->update();
		} else if (handlers[i]) {
			delete handlers[i];
			handlers[i] = 0;
		}
	}
#endif // SOUND_CHANNELS
}

void Sound::setVolume(int8_t volume) {
	globalVolume = (volume < 0) ? volumeMax : volume % (volumeMax+1); //wrap volume value
}

uint8_t Sound::getVolume() {
	return globalVolume;
}

#if SOUND_CHANNELS > 0

#ifdef __cplusplus
extern "C" {
#endif
void Audio_Handler (void) __attribute__((optimize("-O3")));

void Audio_Handler (void) {
	int16_t output = 0;
	for (uint8_t i = 0; i < SOUND_CHANNELS; i++) {
		if (channels[i].use) {
			if (channels[i].index < channels[i].total - 1) {
				output += (channels[i].buffer[channels[i].index++] - 0x80);
			} else if (!channels[i].last) {
				channels[i].index = 0;
			} else if (channels[i].loop) {
				handlers[i]->rewind();
			} else {
				channels[i].use = false;
			}
		}
	}
	if (output) {
		analogWrite(A0, (output + 0x100));
	}
	TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}

void TC5_Handler (void) __attribute__ ((weak, alias("Audio_Handler")));

#ifdef __cplusplus
}
#endif

#endif // SOUND_CHANNELS

} // Gamebuino_Meta
