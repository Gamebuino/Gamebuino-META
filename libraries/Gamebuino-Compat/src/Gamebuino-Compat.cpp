#include "Gamebuino-Compat.h"

namespace Gamebuino_Compat {

void Gamebuino::begin() {
	Gamebuino_Meta::Gamebuino::begin();
	EEPROM.begin(1024);
	sound.begin();
}

void Gamebuino::getDefaultName(char* string) {
	char name[13];
	Gamebuino_Meta::Gamebuino::getDefaultName(name);
	memcpy(string, name, 10);
	string[10] = '\0';
}

}; // namespace Gamebuino_Compat

Gamebuino gb;
