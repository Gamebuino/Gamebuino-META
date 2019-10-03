// Wire Slave Sender
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include <Gamebuino-Meta.h>

void setup()
{
  gb.begin();
  Wire.begin(2);                // join i2c bus with address #2

}

void loop()
{
  while (!gb.update());
  gb.display.clear();

  //Display on screen
  gb.display.print("I2C Test\n");
  gb.display.print("\n\n");
  gb.display.print("Press A on the other\n");
  gb.display.print("GAMEBUINO\n\n");
  //Wait for a request
  Wire.onRequest(requestEvent); // register event
  delay(500);

}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write("hello "); // respond with message of 6 bytes as expected by master
  gb.display.print("Sending Hello");

}
