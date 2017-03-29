#ifndef _GAMEBUINO_SD_GFX_H_
#define	_GAMEBUINO_SD_GFX_H_

#include <Arduino.h>
#include "../Graphics.h"
#include "../Image.h"

namespace Gamebuino_Meta {

#define BUFFPIXEL 16

class Gamebuino_SD_GFX{
	public:
	void begin();
	static uint8_t writeImage(Image img, char *filename);
	static uint8_t readImage(Image img, char *filename);
	static Print* debugOutput;
};

} // namespace Gamebuino_Meta

#endif /// _GAMEBUINO_SD_GFX_H_
