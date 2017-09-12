#ifndef _GAMEBUINO_META_TONE_H_
#define _GAMEBUINO_META_TONE_H_

#include "Sound.h"

namespace Gamebuino_Meta {

class Sound_Handler_Tone : public Sound_Handler {
public:
	Sound_Handler_Tone(Sound_Channel* chan, uint32_t frequency, int32_t duration, int8_t i);
	~Sound_Handler_Tone();
	void update();
	void rewind();
private:
	int8_t identifier;
};

}; // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_TONE_H_
