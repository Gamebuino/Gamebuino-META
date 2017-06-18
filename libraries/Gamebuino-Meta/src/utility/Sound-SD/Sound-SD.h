/*
 * Copyright (c) 2015 by 
 Arturo Guadalupi <a.guadalupi@arduino.cc>
 Angelo Scialabba <a.scialabba@arduino.cc>
 Claudio Indellicati <c.indellicati@arduino.cc> <bitron.it@gmail.com>
 
 * Audio library for Arduino Zero.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */
	
#ifndef _GAMEBUINO_META_SOUND_SD_H_
#define _GAMEBUINO_META_SOUND_SD_H_

#include <Arduino.h>

namespace Gamebuino_Meta {

class Sound_SD {
public:

	Sound_SD() {};
	void begin(uint32_t sampleRate);
//	void prepare(int volume); //not implemented yet
	void play(char* filename) ;
	void end();
	void update();

private:
	void dacConfigure(void);
	void tcConfigure(uint32_t sampleRate);
	bool tcIsSyncing(void);
	void tcStartCounter(void);
	void tcReset(void);
	void tcEnable(void);
	void tcDisable(void);
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_SOUND_SD_H_
