#include "Gamebuino-Compat.h"

namespace Gamebuino_Compat {

void Gamebuino::begin() {
	Gamebuino_Meta::Gamebuino::begin();
	EEPROM.begin(1024);
}

}; // namespace Gamebuino_Compat

Gamebuino gb;
