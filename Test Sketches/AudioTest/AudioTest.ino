#include <Gamebuino-Meta.h>

void setup() {
	gb.begin();
	gb.titleScreen("Audio Test");
}

int8_t music = -1;
int8_t fx = -1;

void loop() {
	if (gb.update()) {
		gb.display.print("RAM:");
		gb.display.println(gb.getFreeRam());
		gb.display.print("CPU:");
		gb.display.print(gb.getCpuLoad());
		gb.display.println("%");
		if (music != -1) {
			gb.display.println("Playing Music");
		}
		if (!gb.sound.isPlaying(fx)) {
			fx = -1;
		}
		if (fx != -1) {
			gb.display.println("Playing Sound-Effect");
		}
		if (gb.buttons.pressed(BUTTON_A)) {
			if (music == -1) {
				music = gb.sound.play("test.wav", true); // true for infinite looping
			} else {
				gb.sound.stop(music);
				music = -1;
			}
		}
		if (gb.buttons.pressed(BUTTON_B)) {
			if (fx == -1) {
				fx = gb.sound.playOK();
			}
		}
	}
}
