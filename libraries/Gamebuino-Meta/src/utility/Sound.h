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

#ifndef SOUND_H
#define	SOUND_H

#include <Arduino.h>


class Sound {
public:
	void begin(){
		volumeMax = 1;
		globalVolume = 0;
	};
	void playOK(){
		if(globalVolume)
			tone(A0,1000,50);
	};
	void playCancel(){
		if(globalVolume)
			tone(A0,500,50);
	};
	void playTick(){
		if(globalVolume)
			tone(A0,1000,5);
	};
	
	void setVolume(int8_t volume) {
		globalVolume = (volume < 0) ? volumeMax : volume % (volumeMax+1); //wrap volume value
	};
	uint8_t getVolume() {
		return globalVolume;
	};
	
	uint8_t globalVolume;
	uint8_t volumeMax;
};

#endif	/* SOUND_H */

