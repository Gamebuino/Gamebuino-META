
/*
   IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
   An IR LED must be connected to Arduino PWM pin 3.
   Version 0.1 July, 2009
   Copyright 2009 Ken Shirriff
   http://arcfn.com
*/

#include <Gamebuino-Meta.h>
#include <IRremote.h>


IRsend irsend(A3);

void setup()
{
  gb.begin();

}

void loop() {
  while (!gb.update());
  gb.display.clear();

  //print it on screen
  gb.display.print("INFRARED EMMITER \n ");
  gb.display.print("\n ");
  gb.display.print("Press A to send A:\n");

  //If the user clicks on the button A, a signal is sent
  if (gb.buttons.repeat(BUTTON_A, 0)) {
    irsend.sendSony(0xa, 12);
    //delay(10);
    gb.display.print("SENDING");
  }
  delay(100);
}
