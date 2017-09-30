#ifndef _GAMEBUINO_META_SOUND_RAW_H_
#define _GAMEBUINO_META_SOUND_RAW_H_

#include "Sound.h"

namespace Gamebuino_Meta {

class Sound_Handler_Raw : public Sound_Handler {
public:
	Sound_Handler_Raw(Sound_Channel* chan, uint8_t* buffer, uint32_t lenght);
	void update();
	void rewind();
private:
	uint8_t* buffer;
	uint32_t length;
};

}; // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_SOUND_RAW_H_
