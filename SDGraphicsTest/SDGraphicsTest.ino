#include <Gamebuino-Meta.h>

Image img = Image(80, 64, ColorMode::rgb565);
void setup() {
	gb.begin();
	if (!gb.sd_gfx.playImage(img, "/TMP0008.BIN")) {
		while(1);
	}
	/*
	if (!gb.sd_gfx.readImage(img, "/uforace/SCREEN.BMP")) {
		while(1);
	}
	*/
}
uint32_t frames = 0;
void loop() {
	if (gb.update()) {
		gb.display.drawImage(0, 0, img);
	}
}
