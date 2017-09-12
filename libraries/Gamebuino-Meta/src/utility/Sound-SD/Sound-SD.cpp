/*
 * Copyright (c) 2015 by 
 Arturo Guadalupi <a.guadalupi@arduino.cc>
 Angelo Scialabba <a.scialabba@arduino.cc>
 Claudio Indellicati <c.indellicati@arduino.cc> <bitron.it@gmail.com>
 
 * Audio library for Arduino Zero.
 * Modified for usage with Gamebuino Meta
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "Sound-SD.h"
#include "../SdFat.h"
#include "../Misc.h"
#include "../../config/config.h"
extern SdFat SD;

namespace Gamebuino_Meta {

struct Buffers {
	uint8_t* buf = 0;
	bool use = false;
};
Buffers buffers[SOUND_CHANNELS];

const uint16_t NUM_SAMPLES = 1024*2;

Sound_Handler_Wav::Sound_Handler_Wav(Sound_Channel* chan) : Sound_Handler(chan) {
	
};

Sound_Handler_Wav::~Sound_Handler_Wav() {
	if (file) {
		file.close();
	}
	if (channel->buffer) {
		for (uint8_t i = 0; i < SOUND_CHANNELS; i++) {
			if (buffers[i].buf == channel->buffer) {
				buffers[i].use = false;
			}
		}
	}
}

bool Sound_Handler_Wav::init(const char* filename) {
	file = SD.open(filename);
	if (!file) {
		return false;
	}
	file.rewind();
	if (f_read32(&file) != 0x46464952) { // header "RIFF"
		return false;
	}
	file.seekCur(4); // skip ChunkSize
	if (f_read32(&file) != 0x45564157) { // header "WAVE"
		return false;
	}
	wav_offset = 16;
	while (1) {
		// read chunks until we get to the data chunk
		uint32_t type = f_read32(&file);
		uint32_t size = f_read32(&file);
		if (type == 0x61746164) { // type "data", we have found our thing
			break;
		}
		if (!file.seekCur(size)) {
			file.close();
			return false;
		}
		wav_offset += 8 + size;
	}
	// ok stuff is OK, let's set things
	channel->index = 0;
	channel->total = NUM_SAMPLES;
	channel->buffer = 0;
	for (uint8_t i = 0; i < SOUND_CHANNELS; i++) {
		if (!buffers[i].use) {
			if (!buffers[i].buf) {
				buffers[i].buf = (uint8_t*)malloc(NUM_SAMPLES);
			}
			buffers[i].use = true;
			channel->buffer = buffers[i].buf;
			break;
		}
	}
	if (!channel->buffer) {
		return false;
	}
	channel->total = NUM_SAMPLES;
	channel->last = false;
	channel->type = Sound_Channel_Type::raw;
	head_index = 0;
	rewind_flag = false;
	
	// fill the initial buffer
	
	file.read(channel->buffer, NUM_SAMPLES);
	channel->use = true;
}

void Sound_Handler_Wav::rewind() {
	rewind_flag = true;
}

void Sound_Handler_Wav::update() {
	if (rewind_flag) {
		rewind_flag = false;
		file.seekSet(wav_offset);
		channel->total = NUM_SAMPLES;
		channel->index = 0;
		channel->last = false;
		head_index = 0;
		file.read(channel->buffer, NUM_SAMPLES);
		return;
	}
	if (channel->last || !file.available()) {
		channel->last = true;
		channel->total = head_index; // well we can't output more anyways...
		return;
	}
	uint32_t current__SampleIndex = channel->index;
	if (current__SampleIndex > head_index) {
		file.read(&(channel->buffer[head_index]), current__SampleIndex - head_index);
	} else {
		file.read(&(channel->buffer[head_index]), NUM_SAMPLES-1 - head_index);
		file.read(channel->buffer, current__SampleIndex);
	}
	head_index = current__SampleIndex;
}

} // namespace Gamebuino_Meta
