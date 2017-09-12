/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *é
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

#ifndef _GAMEBUINO_META_SOUND_H_
#define	_GAMEBUINO_META_SOUND_H_

#include <Arduino.h>

namespace Gamebuino_Meta {

enum class Sound_Channel_Type : uint8_t {
	raw,
	square,
};

class Sound_Handler;
struct Sound_Channel {
	uint16_t index;
	uint16_t total;
	uint8_t* buffer;
	uint8_t amplitude;
	bool loop;
	bool last;
	bool use = false;
	Sound_Handler* handler = 0;
	Sound_Channel_Type type;
};

class Sound_Handler {
public:
	Sound_Handler(Sound_Channel* chan);
	virtual ~Sound_Handler();
	virtual void update() = 0;
	virtual void rewind() = 0;
	void setChannel(Sound_Channel* chan);
protected:
	Sound_Channel* channel;
};

class Sound {
public:
	void begin();
	int8_t play(const char* filename, bool loop = false);
	int8_t play(char* filename, bool loop = false);
	int8_t play(const uint16_t* buf, bool loop = false);
	int8_t play(uint16_t* buf, bool loop = false);
	int8_t play(const uint16_t** buf, bool loop = false);
	int8_t play(uint16_t** buf, bool loop = false);
	int8_t play(Sound_Handler* handler, bool loop = false);
	int8_t tone(uint32_t frequency, int32_t duration = 0);
	int8_t playOK();
	int8_t playCancel();
	int8_t playTick();
	
	bool isPlaying(int8_t i);
	
	void update();
	void mute();
	void unmute();
	bool isMute();
	
	void startEfxOnly();
	void stopEfxOnly();
	
	void stop(int8_t i);
	
	void setVolume(uint8_t volume);
	uint8_t getVolume();
};

} // namespace Gamebuino_Meta

#endif	// _GAMEBUINO_META_SOUND_H_
