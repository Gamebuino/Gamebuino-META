// import the Gamebuino library
#include <Gamebuino-Meta.h>

// set our save slot for a number
#define SAVE_NUMBER 0
// set our save slot for a string
#define SAVE_STRING 1

// these variables will be used to cache our save variables in ram
// ACCESSING SAVE VARIABLES IS SLOW AND NOT TO BE USED IN A LOOP REPETITIVELY
char string[40];
int number;


// gamebuino setup
void setup() {
  gb.begin();
  
  // let's load our save variables!
  // if not set, number defaults to zero
  number = gb.save.get(SAVE_NUMBER);
  // if not set, string will be filled with zeros
  gb.save.get(SAVE_STRING, string);
}

void loop() {
  while(!gb.update());
  
  // clear the previous screen
  gb.display.clear();
  
  // let's first output our cached stuff
  gb.display.println("Number:");
  gb.display.println(number);
  gb.display.println("String:");
  gb.display.println(string);
  
  if (gb.buttons.pressed(BUTTON_UP)) {
    // first increase our cached variable
    number++;
    // now save it to our save slot!
    gb.save.set(SAVE_NUMBER, number);
    gb.sound.playTick();
  }
  if (gb.buttons.pressed(BUTTON_DOWN)) {
    // first decrease our cached variable
    number--;
    // now save it to our save slot!
    gb.save.set(SAVE_NUMBER, number);
    gb.sound.playTick();
  }
  
  if (gb.buttons.pressed(BUTTON_A)) {
    // okay, let's let the user modify our cached string variable
    gb.keyboard(string, 40);
    // aaaaaand now save it to its slot!
    gb.save.set(SAVE_STRING, string);
  }
  
  if (gb.buttons.pressed(BUTTON_B)) {
    // first we delete both of our save slots
    gb.save.del(SAVE_NUMBER);
    gb.save.del(SAVE_STRING);
    
    // and now we reset our cached variables
    // alternatively we could fetch the defaults with gb.save.get, too
    number = 0;
    memset(string, 0, sizeof(string));
    gb.sound.playCancel();
  }
}
