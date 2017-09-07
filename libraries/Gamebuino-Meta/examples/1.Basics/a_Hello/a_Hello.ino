//imports the Gamebuino library and the gb object
#include <Gamebuino-Meta.h>

// the setup routine runs once when Gamebuino starts up
void setup(){
  // initialize the Gamebuino object
  gb.begin();
  //display the main menu:
  gb.titleScreen("My first game");
  //creates a popup that will last for 100 frames (4 seconds)
  gb.popup("Let's go!", 100);
}

// the loop routine runs over and over again forever
void loop(){
  //updates the gamebuino (the display, the sound... everything)
  //returns true when it's time to render a new frame (25 times/second)
  if(gb.update()){
    //prints Hello World! on the screen
    gb.display.println("Hello World!");
    //declare a variable named count of type integer :
    int count;
    //get the number of frames rendered and assign it to the "count" variable
    count = gb.frameCount;
    //prints the variable "count"
    gb.display.println(count);
  }
}
