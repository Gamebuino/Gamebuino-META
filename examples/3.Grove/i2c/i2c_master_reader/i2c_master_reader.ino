// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include <Gamebuino-Meta.h>

// Two external pull-up resistors are plugged in.

void setup()
{
  gb.begin();
  Wire.begin();        // join i2c bus (address optional for master)

}

void loop()
{
  while (!gb.update());
  gb.display.clear();


  //Display on screen
  gb.display.print("I2C Test\n");
  gb.display.print("\n");
  gb.display.print("A to receive a msg");
  gb.display.print("\n\n");

  // When button A is pressed, a request is sent to the other gamebuino
  if (gb.buttons.repeat(BUTTON_A, 0)) {
    gb.display.print("REQUESTING..\n");
    gb.display.print("\n");
    if (Wire.requestFrom(2, 6)) {   // request 6 bytes from slave device #2
      //delay(10);
      gb.display.print("RECEIVED : ");
    }
  }

  //When the other gamebuino received the request, it sends "Hello"
  while (Wire.available())   // slave may send less than requested
  {

    char c = Wire.read(); // receive a byte as character
    gb.display.print(c);         // print the character
  }

  delay(500);
}
