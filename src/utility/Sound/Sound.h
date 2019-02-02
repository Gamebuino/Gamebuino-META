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
 - Aurelien Rodot
 - Sorunome
*/

#ifndef _GAMEBUINO_META_SOUND_H_
#define	_GAMEBUINO_META_SOUND_H_

#include "../../config/config.h"
#include "Sound_FX.h"

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
	virtual uint32_t getPos();
	void setChannel(Sound_Channel* chan);
protected:
	Sound_Channel* channel;
};

// Pre declare Sound_Handler_FX for the FX_Channel
class Sound_Handler_FX;
struct Sound_FX;

struct FX_Channel {
	int8_t * buffer;
	uint16_t index;
	Sound_Handler_FX * handler;
	uint16_t size;
};

class Sound {
public:
	void begin();
	int8_t play(const char* filename, bool loop = false);
	int8_t play(char* filename, bool loop = false);
	int8_t play(const uint16_t* buf, bool loop = false);
	int8_t play(uint16_t* buf, bool loop = false);
	int8_t play(const uint8_t* buf, uint32_t len, bool loop = false);
	int8_t play(uint8_t* buf, uint32_t len, bool loop = false);
	template<uint32_t N>
	int8_t play(const uint8_t (&buf)[N], bool loop = false) {
		return play(buf, N, loop);
	};
	template<uint32_t N>
	int8_t play(uint8_t (&buf)[N], bool loop = false) {
		return play((const uint8_t*)buf, N, loop);
	};
	
	int8_t play(Sound_Handler* handler, bool loop = false);

	void fx(const Sound_FX & fx);
	void fx(const Sound_FX * const fx);

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
	
	uint32_t getPos(int8_t i);
};

} // namespace Gamebuino_Meta

#endif	// _GAMEBUINO_META_SOUND_H_
