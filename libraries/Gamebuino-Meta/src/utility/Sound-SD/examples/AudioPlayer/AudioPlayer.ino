#include <Gamebuino-Meta.h>
#include <utility/Sound-SD.h>
Gamebuino_Meta::Sound_SD AudioZero;

char name[50];

void setup() {
	gb.begin();
	AudioZero.begin(44100);
	//AudioZero.begin(8000);
	//gb.sound.setVolume(1);
	//gb.sound.playTick();
	//while(1);
	gb.tft.println("Starting to play sound file...");

	AudioZero.play("/test.wav");
}

void loop() {
	AudioZero.update();
	/*if (gb.update()) {
		gb.display.println("Hatsune Miku");
		gb.display.println(gb.frameCount);
		gb.display.print("RAM:");
		gb.display.println(gb.getFreeRam());
	}*/
}