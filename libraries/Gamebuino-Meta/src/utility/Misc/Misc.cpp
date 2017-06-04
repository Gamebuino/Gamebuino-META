#include "Misc.h"

namespace Gamebuino_Meta {

// create a unique path thing
// call via etc.
// char name[] = "/TMP0000.BIN";
// sd_path_no_duplicate(name, 4, 4); // returns true on success, false on no success
bool sd_path_no_duplicate(char* name, uint8_t offset, uint8_t digits) {
	uint32_t power = 1;
	for (uint8_t j = 0; j < digits; j++) {
		power *= 10;
	}
	for(uint32_t i = 0; i < power; i++) {
		// in this loop we copy i to the buffer
		char* buf = name + offset + digits - 1;
		uint32_t j = i;
		while (true) {
			*(buf) = '0' + (j % 10);
			if (j < 10) {
				break;
			}
			j /= 10;
			buf--;
		}
		if (!SD.exists(name)) {
			return true; // we are done folks!
		}
	}
}

uint16_t f_read16(File* f) {
	uint16_t result;
	f->read(&result, 2);
	return result;
}

uint32_t f_read32(File* f) {
	uint32_t result;
	f->read(&result, 4);
	return result;
}

void f_write32(uint32_t b, File* f) {
	//Write four bytes
	//Luckily our MCU is little endian so byte order like this is fine
	f->write(&b, 4);
}

void f_write16(uint16_t b, File* f) {
	//Write two bytes
	//Luckily our MCU is little endian so byte order like this is fine
	f->write(&b, 2);
}


} // namespace Gamebuino_Meta
