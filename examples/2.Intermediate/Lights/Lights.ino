// === LIGHTS === //
// gb.light is a 2x4 pixel image to which we can draw to
// drawing stuff to it will automatically update the lights in the background

// import the Gamebuino library
#include <Gamebuino-Meta.h>


bool lightsOn = false;
bool redLight = false;

void setup() {
  gb.begin();
}

void loop() {
  while(!gb.update());
  
  // clear the previous screen
  gb.display.clear();
  
  // clear the previous state of the lights
  gb.lights.clear();
  
  if (lightsOn) {
    // if we want the lights to be on, let's turn all lights white!
    gb.lights.fill(WHITE);
  }
  
  if (redLight) {
    // if we want the red light, let's set the lower-left light red
    // if lightsOn is true we will just over-write the lower-left light, and thus the lower-left light **WILL** be red for sure!
    gb.lights.drawPixel(0, 3, RED);
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
  gb.display.println("LIGHTS\nA: TOGGLE ALL\nB: TOGGLE RED");
}