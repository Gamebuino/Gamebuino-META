#include "EEPROM_Compat.h"

#include "../Gamebuino-Compat.h"

namespace Gamebuino_Compat {

void EEPROM_Class::begin(uint32_t _size) {
	size = _size;
	buffer = (uint8_t*)malloc(size);
	blocks = size / SAVECONF_DEFAULT_BLOBSIZE;
	if (size % SAVECONF_DEFAULT_BLOBSIZE) {
		blocks++;
	}
	gb.display.fillScreen(WHITE);
	gb.display.setCursors(0, 0);
	gb.display.setColor(BLACK);
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
		flush();
	}
}

void EEPROM_Class::update(uint32_t address, uint8_t value) {
	write(address, value);
}

template< typename T > T& EEPROM_Class::get(uint32_t address, T& var) {
	if (address >= (size + sizeof(T) - 1)) {
		return var;
	}
	uint8_t *ptr = (uint8_t*) &var;
	memcpy(ptr, &buffer[address], sizeof(T));
	return var;
}

template< typename T > T& EEPROM_Class::put(uint32_t address, T& var) {
	if (address >= (size + sizeof(T) - 1)) {
		return var;
	}
	uint8_t *ptr = (uint8_t*) &var;
	memcpy(&buffer[address], ptr, sizeof(T));
	flush();
	return var;
}

}; // namespace Gamebuino_Compat

EEPROM_Class EEPROM;
