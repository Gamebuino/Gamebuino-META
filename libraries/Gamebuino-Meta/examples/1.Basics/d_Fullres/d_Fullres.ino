#include <Gamebuino.h>

void setup() {
	gb.begin();
	gb.display.ColorMode = ColorMode::INDEX;
}
void loop() {
	if (gb.update()) {
		// do stuff
	}
}
