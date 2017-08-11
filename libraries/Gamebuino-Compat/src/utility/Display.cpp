#include "Display.h"

namespace Gamebuino_Compat {

Display::Display() : Gamebuino_Meta::Image(80, 64, ColorMode::rgb565) {
	
}

void Display::clear() {
	fillScreen(WHITE);
	setCursors(0, 0);
}

}; // namespace Gamebuino_Compat
