//imports the Gamebuino library and the gb object
#include <Gamebuino-Meta.h>

// the setup routine runs once when Gamebuino starts up
void setup(){
  // initialize the Gamebuino object
  gb.begin();
}

// the loop routine runs over and over again forever
void loop(){
  // wait until the gamebuino is ready to update at stable 25 FPS
  // this also updates sounds, button presses... everything!
  while(!gb.update());
  
  // clear the previous screen
  gb.display.clear();
  
  //prints Hello World! on the screen
  gb.display.println("hello world!");
  //declare a variable named count of type integer :
  int count;
  //get the number of frames rendered and assign it to the "count" variable
  count = gb.frameCount;
  //prints the variable "count"
  gb.display.println(count);
}