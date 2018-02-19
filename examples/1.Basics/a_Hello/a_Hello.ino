//imports the Gamebuino library and the gb object
#include <Gamebuino-Meta.h>

// the setup routine runs once when Gamebuino starts up
void setup(){
  // initialize the Gamebuino object
  gb.begin();
  //creates a popup that will last for 100 frames (4 seconds)
  gb.popup("Let's go!", 100);
}

// the loop routine runs over and over again forever
void loop(){
  // wait until the gamebuino is ready to update at stable 25 FPS
  // this also updates sounds, button presses....everything!
  while(!gb.update());
  
  // clear the previous screen
  gb.display.clear();
  //prints Hello World! on the screen
  gb.display.println("Hello World!");
  //declare a variable named count of type integer :
  int count;
  //get the number of frames rendered and assign it to the "count" variable
  count = gb.frameCount;
  //prints the variable "count"
  gb.display.println(count);
}
