#ifndef _GAMEBUINO_COMPAT_BATTERY_H_
#define _GAMEBUINO_COMPAT_BATTERY_H_

#include <Arduino.h>


namespace Gamebuino_Compat {

class Battery {
public:
	void begin();
	void update();
	bool show;
	uint8_t  level;
	uint16_t voltage;
	uint16_t thresholds[4];
	uint8_t  nextUpdate;
};

}; // namespace Gamebuino_Compat


#endif // _GAMEBUINO_COMPAT_BATTERY_H_
