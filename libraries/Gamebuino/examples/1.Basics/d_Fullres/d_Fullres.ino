#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup() {
	gb.begin();
	gb.display.ColorMode = ColorMode::INDEX;
}
void loop() {
	if (gb.update()) {
		// do stuff
	}
}
