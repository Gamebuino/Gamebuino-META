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
