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

#ifndef _GAMEBUINO_EEPROM_H_
#define _GAMEBUINO_EEPROM_H_

#include <Arduino.h>

namespace Gamebuino_EEPROM {

class EEPROM_Class {
private:
	uint8_t* buffer;
	void flush();
	void flush(uint32_t address, uint32_t size);
	uint32_t blocks;
	uint32_t size;
public:
	void begin(uint32_t _size);
	uint8_t read(uint32_t address);
	void write(uint32_t address, uint8_t value);
	void update(uint32_t address, uint8_t value);
	
	// for some reason the template ones need to be defined in here
	template< typename T > T& get(uint32_t address, T& var) {
		if (address >= (size + sizeof(T) - 1)) {
			return var;
		}
		uint8_t *ptr = (uint8_t*) &var;
		memcpy(ptr, &buffer[address], sizeof(T));
		return var;
	}
	
	template< typename T > T& put(uint32_t address, T& var) {
		if (address >= (size + sizeof(T) - 1)) {
			return var;
		}
		uint8_t *ptr = (uint8_t*) &var;
		if (memcmp(&buffer[address], ptr, sizeof(T))) {
			memcpy(&buffer[address], ptr, sizeof(T));
			flush(address, sizeof(T));
		}
		return var;
	}
	
	uint8_t put(uint32_t address, uint8_t var);
};

};

extern Gamebuino_EEPROM::EEPROM_Class EEPROM;

#endif // _GAMEBUINO_EEPROM_H_
