#include <Gamebuino-Meta.h>

void setup() {
  gb.begin();
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
      gb.display.println("Playing pattern");
    }
    
    //bottom line indications
    gb.display.setColor(BROWN);
    gb.display.cursorX = 0;
    gb.display.cursorY = gb.display.height() - 5;
    gb.display.setColor(GREEN);
    gb.display.print("A");
    gb.display.setColor(BROWN);
    gb.display.print("MUSIC");
    gb.display.setColor(RED);
    gb.display.print(" B");
    gb.display.setColor(BROWN);
    gb.display.print("PATTERN");

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
