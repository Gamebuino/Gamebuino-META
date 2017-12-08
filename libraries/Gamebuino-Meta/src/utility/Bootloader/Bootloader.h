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
	void lock();
	void unlock();
	void flash_delete(uint32_t addr);
	void flash_write(uint32_t size, uint32_t *src_addr, uint32_t *dst_addr);
};

}; // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_BOOTLOADER_H_
