
#ifndef _GAMEBUINO_META_MISC_H_
#define _GAMEBUINO_META_MISC_H_

#include "../SdFat.h"

extern SdFat SD;

namespace Gamebuino_Meta {

int32_t sdPathNoDuplicate(char* name, uint8_t offset, uint8_t digits, int32_t start = 0);

struct RGB888 {
	uint8_t r, g, b;
};

uint16_t rgb888Torgb565(RGB888 c);
RGB888 rgb565Torgb888(uint16_t c);

uint16_t f_read16(File* f);

uint32_t f_read32(File* f);

void f_write32(uint32_t b, File* f);

void f_write16(uint16_t b, File* f);

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_MISC_H_
