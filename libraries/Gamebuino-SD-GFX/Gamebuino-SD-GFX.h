#ifndef GAMEBUINO_SD_GFX_H
#define	GAMEBUINO_SD_GFX_H

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Image.h"
#include "SD.h"

#define BUFFPIXEL 20

class Gamebuino_SD_GFX{
	public:
	void begin();
	static uint8_t writeImage(Image img, char *filename);
	static uint8_t readImage(Image img, char *filename);
	static Print* debugOutput;
};

#endif /* GAMEBUINO-SD-GFX_H */