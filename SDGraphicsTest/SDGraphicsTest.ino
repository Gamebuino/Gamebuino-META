#include <Gamebuino-Meta.h>

Image img = Image(80, 64, "/uforace/SCREEN.BMP");
void setup() {
	gb.begin();
	//img.init();
	/*
	if (!gb.sd_gfx.readImage(img, "/uforace/SCREEN.BMP")) {
		while(1);
	}
	*/
}

void loop() {
//	Image img = Image("/uforace/SCREEN.BMP");
	if (gb.update()) {
		gb.display.drawImage(0, 0, img);
		gb.display.setColor(WHITE, BLACK);
		gb.display.println(img.frames);
	}
}
