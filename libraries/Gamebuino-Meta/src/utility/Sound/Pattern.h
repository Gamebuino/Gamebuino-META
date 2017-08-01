#ifndef _GAMEBUINO_META_PATTERN_H_
#define _GAMEBUINO_META_PATTERN_H_

#include "Sound.h"

namespace Gamebuino_Meta {

class Sound_Handler_Pattern : public Sound_Handler {
public:
	Sound_Handler_Pattern(Sound_Channel* chan, uint8_t* buffer);
	void update();
	void rewind();
private:
	uint8_t patternCursor;
	uint8_t note_duration;
};

}; // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_PATTERN_H_
