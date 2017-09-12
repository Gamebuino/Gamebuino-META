//importe the Gamebuino library and the gb object
#include <Gamebuino-Meta.h>

// gamebuino setup
void setup() {
  gb.begin();
}

// these will be aour track identifiers. -1 means "not playing"
int8_t music = -1;
int8_t fx = -1;

void loop() {
  if (gb.update()) {
    // let's just print some information on free ram and CPU first
    gb.display.print("RAM:");
    gb.display.println(gb.getFreeRam());
    gb.display.print("CPU:");
    gb.display.print(gb.getCpuLoad());
    gb.display.println("%");
    
    // if we have music playing...let's print that!
    if (music != -1) {
      gb.display.println("Playing Music");
    }
    
    // here we check if the sound effects stopped playing.
    // If they did, we will set our sound effect identifier back to -1
    if (!gb.sound.isPlaying(fx)) {
      fx = -1;
    }
    // and now, only print that an effect is playing
    if (fx != -1) {
      gb.display.println("Playing Effect");
    }
    
    // bottom line indications
    // this just prints the lower bar for what button does what
    gb.display.setCursors(0, gb.display.height() - 5);
    gb.display.setColor(GREEN);
    gb.display.print("A");
    gb.display.setColor(BROWN);
    gb.display.print("MUSIC");
    gb.display.setColor(RED);
    gb.display.print(" B");
    gb.display.setColor(BROWN);
    gb.display.print("EFFECT");

    // okay, now we check if we want to start music
    if (gb.buttons.pressed(BUTTON_A)) {
      if (music == -1) {
        // time to start music!
        // gb.sound.play() will return the track identifier, which we will store into our music variable for later use
        music = gb.sound.play("test.wav", true); // true for infinite looping
      } else {
        // music was already running, time to stop it
        gb.sound.stop(music);
        music = -1; // aaaaaand reset our track indicator
      }
    }
    
    // check if we want to start a sound effect
    if (gb.buttons.pressed(BUTTON_B)) {
      // we only want to start it if it isn't already running...
      if (fx == -1) {
        // gb.sound.playOK() also returns the track identifier
        fx = gb.sound.playOK();
      }
    }
  }
}
