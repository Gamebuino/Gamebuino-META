#ifndef _GAMEBUINO_META_SOUND_SD_H_
#define _GAMEBUINO_META_SOUND_SD_H_

#include "../Sound.h"
#include "../SdFat.h"

namespace Gamebuino_Meta {

class Sound_Handler_Wav : public Sound_Handler {
public:
	Sound_Handler_Wav(Sound_Channel* chan);
	~Sound_Handler_Wav();
	bool init(const char* filename);
	void update();
	void rewind();
private:
	File file;
	uint16_t head_index;
	uint32_t wav_offset;
	bool rewind_flag;
};

} // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_SOUND_SD_H_
