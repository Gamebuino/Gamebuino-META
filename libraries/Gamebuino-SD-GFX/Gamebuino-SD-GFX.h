#ifndef GAMEBUINO_SD_GFX_H
#define	GAMEBUINO_SD_GFX_H

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Image.h"
#include "SD.h"

class Gamebuino_SD_GFX{
	public:
	void begin();
	uint8_t writeImage(Image img, char *filename);
	uint8_t readImage(Image img, char *filename);
};

#endif /* GAMEBUINO-SD-GFX_H */