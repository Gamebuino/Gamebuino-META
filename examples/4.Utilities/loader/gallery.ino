#include <utility/Misc.h>

void galleryView(int8_t direction) {
	uint32_t max = 0;
	strcpy(nameBuffer, getCurrentGameFolder());
	strcpy(nameBuffer + strlen(nameBuffer), "/REC/REC.CACHE");
	if (SD.exists(nameBuffer)) {
		File cache = SD.open(nameBuffer);
		cache.rewind();
		cache.read(&max, 4);
		cache.close();
	}
	
	strcpy(nameBuffer, getCurrentGameFolder());
	strcpy(nameBuffer + strlen(nameBuffer), "/REC/00000.GMV");
	uint8_t offset = strlen(nameBuffer) - 5;
	uint8_t offsetStart = offset - 4;
	
	int32_t index;
	if (direction > 0) {
		index = -1;
	} else {
		index = max + 1;
	}
	
	bool loaded = false;
	while(1) {
		while(!gb.update());
		gb.display.nextFrame();
		if (!loaded) {
			do {
				index += direction;
				if (index < 0 || index > max) {
					return;
				}
				memset(nameBuffer + offsetStart, '0', 5);
				Gamebuino_Meta::intToStr(index, nameBuffer + offset);
			} while(!SD.exists(nameBuffer));
			gb.display.init(nameBuffer);
			loaded = true;
		}
		if (gb.buttons.repeat(BUTTON_DOWN, 4)) {
			direction = 1;
			loaded = false;
		}
		if (gb.buttons.repeat(BUTTON_UP, 4)) {
			direction = -1;
			loaded = false;
		}
		if (gb.buttons.pressed(BUTTON_B)) {
			return;
		}
	}
}
