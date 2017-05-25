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


} // namespace Gamebuino_Meta
