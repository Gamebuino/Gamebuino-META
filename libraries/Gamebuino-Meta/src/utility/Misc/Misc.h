
#ifndef _GAMEBUINO_META_MISC_H_
#define _GAMEBUINO_META_MISC_H_

#include "../SdFat.h"

extern SdFat SD;

namespace Gamebuino_Meta {

bool sd_path_no_duplicate(char* name, uint8_t offset, uint8_t digits);

uint16_t f_read16(File* f);

uint32_t f_read32(File* f);

void f_write32(uint32_t b, File* f);

void f_write16(uint16_t b, File* f);

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_MISC_H_
