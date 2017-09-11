// import the Gamebuino library
#include <Gamebuino-Meta.h>

// gamebuino setup
void setup() {
  gb.begin();
}

// gb.light is a 2x4 pixel image to which we can draw to
// drawing stuff to it will automatically update the lights in the background

bool lightsOn = false;
bool redLight = false;

void loop() {
  if (gb.update()) {
    if (lightsOn) {
      // if we want the lights to be on, let's turn all lights white!
      gb.light.fill(WHITE);
    }
    
    if (redLight) {
      // if we want the red light, let's set the lower-left light red
      // if lightsOn is true we will just over-write the lower-left light, and thus the lower-left light **WILL** be red for sure!
      gb.light.drawPixel(0, 3, RED);
    }
    
    if (gb.buttons.pressed(BUTTON_A)) {
      // toggle lightsOn
      lightsOn = !lightsOn;
    }
    
    if (gb.buttons.pressed(BUTTON_B)) {
      // toggle redLight
      redLight = !redLight;
    }
    
    // now let's just display our controls
    gb.display.println("A: lights on\nB: red light");
  }
}
