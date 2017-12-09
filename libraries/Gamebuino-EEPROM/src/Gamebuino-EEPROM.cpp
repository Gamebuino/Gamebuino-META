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

#include "Gamebuino-EEPROM.h"
#include <Gamebuino-Meta.h>

namespace Gamebuino_EEPROM {

void EEPROM_Class::begin(uint32_t _size) {
	size = _size;
	buffer = (uint8_t*)malloc(size);
	blocks = size / SAVECONF_DEFAULT_BLOBSIZE;
	if (size % SAVECONF_DEFAULT_BLOBSIZE) {
		blocks++;
	}
	gb.display.fill(DISPLAY_DEFAULT_BACKGROUND_COLOR);
	gb.display.setCursor(0, 0);
	gb.display.setColor(DISPLAY_DEFAULT_COLOR);
	gb.display.print("Loading...");
	gb.updateDisplay();
	for (uint32_t i = 0; i < blocks; i++) {
		gb.save.get(i, &buffer[i*SAVECONF_DEFAULT_BLOBSIZE], SAVECONF_DEFAULT_BLOBSIZE);
	}
	flush();
}

void EEPROM_Class::flush() {
	// flush the eeprom buffer to sd card
	for (uint32_t i = 0; i < blocks; i++) {
		gb.save.set(i, &buffer[i*SAVECONF_DEFAULT_BLOBSIZE], SAVECONF_DEFAULT_BLOBSIZE);
	}
}

void EEPROM_Class::flush(uint32_t address, uint32_t size) {
	uint32_t block = address / SAVECONF_DEFAULT_BLOBSIZE;
	int32_t counter = (address % SAVECONF_DEFAULT_BLOBSIZE) + size;
	do {
		gb.save.set(block, &buffer[block*SAVECONF_DEFAULT_BLOBSIZE], SAVECONF_DEFAULT_BLOBSIZE);
		block++;
		counter -= SAVECONF_DEFAULT_BLOBSIZE;
	} while (counter > 0);
}

uint8_t EEPROM_Class::read(uint32_t address) {
	if (address >= size) {
		return 0;
	}
	return buffer[address];
}

void EEPROM_Class::write(uint32_t address, uint8_t value) {
	if (address >= size) {
		return;
	}
	if (buffer[address] != value) {
		buffer[address] = value;
		flush(address, 1);
	}
}

void EEPROM_Class::update(uint32_t address, uint8_t value) {
	write(address, value);
}

uint8_t EEPROM_Class::put(uint32_t address, uint8_t var) {
	update(address, var);
	return var;
}

}; // namespace Gamebuino_Compat

Gamebuino_EEPROM::EEPROM_Class EEPROM;
