#ifndef _GAMEBUINO_COMPAT_H_
#define _GAMEBUINO_COMPAT_H_


#include <Gamebuino-Meta.h>

#include "utility/EEPROM_Compat.h"

#include "utility/Display_Compat.h"
#include "utility/Battery_Compat.h"
#include "utility/Sound_Compat.h"


namespace Gamebuino_Compat {

class Gamebuino : public Gamebuino_Meta::Gamebuino {
public:
	void begin();
	Battery battery;
	void getDefaultName(char* string);
};

} // namespace Gamebuino_Compat


const uint8_t NUM_BTN = 7;
const Button BTN_DOWN = BUTTON_DOWN;
const Button BTN_LEFT = BUTTON_LEFT;
const Button BTN_RIGHT = BUTTON_RIGHT;
const Button BTN_UP = BUTTON_UP;
const Button BTN_A = BUTTON_A;
const Button BTN_B = BUTTON_B;
const Button BTN_C = BUTTON_C;
using Gamebuino_Compat::Gamebuino;

extern Gamebuino_Compat::Gamebuino gb;


#define LCDHEIGHT gb.display.height()
#define LCDWIDTH gb.display.width()

#endif // _GAMEBUINO_COMPAT_H_
