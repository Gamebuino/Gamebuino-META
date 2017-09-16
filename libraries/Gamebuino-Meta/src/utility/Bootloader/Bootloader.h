#ifndef _GAMEBUINO_META_BOOTLOADER_H_
#define _GAMEBUINO_META_BOOTLOADER_H_

#include <Arduino.h>

namespace Gamebuino_Meta {

class Bootloader {
public:
	uint32_t version();
	void game(const char* filename);
	void game(char* filename);
	void loader();
	void enter();
	void error(uint16_t e);
};

}; // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_BOOTLOADER_H_
