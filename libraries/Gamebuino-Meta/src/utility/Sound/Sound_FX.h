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
- Valden
*/

#ifndef _GAMEBUINO_META_SOUND_FX_H_
#define	_GAMEBUINO_META_SOUND_FX_H_


//#include <Gamebuino-Meta.h>
#include "Sound.h"
#include <cstdint>
#include "../../config/config.h"


/*class GSSound_FX
{
public:


static const int32_t CONTINUE_FLAG = (1 << 31);


// Users create and use these
union Sound_FX {
struct {
GSSound_FX::Sound_FX_Wave type; // Type of the wave that will be played
int32_t volume_start; // Volume amplitude as a 8 bit fixed point
int32_t volume_sweep; // How much the volume change for each sample (8 bit fixed point)

int32_t period_start;
int32_t period_sweep;

int32_t length;
};
int32_t params[6];
};

GSSound_FX();
~GSSound_FX();

// Init the sSound_FX library so it can play sound
int8_t init();

// Plays the given Sound_FX structure
void play(const GSSound_FX::Sound_FX & Sound_FX);

// Plays pattern array (Will finish if wave type does not contain the CONTINUE_FLAG flag)
void play(const GSSound_FX::Sound_FX * const pattern);

// DEPRECATED : Plays a Sound_FX array containing length Sound_FXs
void play(const GSSound_FX::Sound_FX * const pattern, uint8_t length);
void play(const GSSound_FX::Sound_FX * const pattern, uint8_t length, uint16_t pitch_scale);


private:
bool is_init;
int8_t _channel_id;



Sound_Handler_GSSound_FX _handler;

};*/


namespace Gamebuino_Meta {

	class Sound_Handler_FX //: public Gamebuino_Meta::Sound_Handler
	{
	public:
		static const uint8_t FPP = 8; // Fixed point precision
		static const uint8_t SR_DIVIDER = 44100 / SOUND_FREQ;
		inline Sound_Handler_FX(FX_Channel* _channel) : parent_channel(_channel)  {
			init();
		};
		~Sound_Handler_FX() {};

		void init();
		void update();
		void rewind();
		void generateNoise() __attribute__((optimize("-O3"))); // Handle noise instrument
		void generateSquare() __attribute__((optimize("-O3")));

		void play(const Gamebuino_Meta::Sound_FX & Sound_FX);
		void play(const Gamebuino_Meta::Sound_FX * const pattern, uint8_t length);
		void play(const Gamebuino_Meta::Sound_FX * const pattern, uint8_t length, uint16_t pitch_scale);

		uint32_t getPos();
		Gamebuino_Meta::Sound_FX _current_Sound_FX;
		Gamebuino_Meta::Sound_FX const * _current_pattern;
		uint8_t _current_pattern_Sound_FX;
		uint8_t _current_pattern_length;

		uint32_t _current_Sound_FX_time;
		int32_t _current_Sound_FX_volume;
		int32_t _current_Sound_FX_freq;
		uint16_t _head_index;
		int16_t _pitch_scale;

		int32_t _noise_period;
		int32_t _square_period;
		int32_t _square_polarity;

		void resetGenerators();

		inline uint8_t getVolume()
		{
			_current_Sound_FX_volume += _current_Sound_FX.volume_sweep * SR_DIVIDER;
			return (min(127, max(0, (_current_Sound_FX_volume >> FPP))));
		} __attribute__((optimize("-O3")));

		inline int32_t getFrequency()
		{
			_current_Sound_FX_freq += _current_Sound_FX.period_sweep * SR_DIVIDER;
			return ((_current_Sound_FX_freq * _pitch_scale) >> FPP);
		} __attribute__((optimize("-O3")));

		FX_Channel * parent_channel;
	};

};


#endif // _GAMEBUINO_META_SOUND_FX_H_