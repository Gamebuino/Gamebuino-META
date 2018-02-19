#include <Gamebuino-Meta.h>

void setup() {
	gb.begin();
}

void loop() {
	while(!gb.update());
	// clear the previous screen
	gb.display.clear();
	
	gb.display.println("A to trigger hard fault");
	gb.display.println(gb.frameCount);
	if (gb.buttons.pressed(BUTTON_A)) {
		// nuke (jumping to a non-aligned location)
		((void(*)(void))(*((uint32_t*)0x3FEC) + 1))();
	}
}
