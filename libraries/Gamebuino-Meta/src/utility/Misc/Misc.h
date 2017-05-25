
#ifndef _GAMEBUINO_META_MISC_H_
#define _GAMEBUINO_META_MISC_H_

#include "../SdFat.h"

extern SdFat SD;

namespace Gamebuino_Meta {

bool sd_path_no_duplicate(char* name, uint8_t offset, uint8_t digits);
	
} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_MISC_H_
