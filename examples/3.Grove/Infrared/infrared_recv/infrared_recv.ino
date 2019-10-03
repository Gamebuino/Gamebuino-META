/*
   IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
   An IR detector/demodulator must be connected to the input RECV_PIN.
   Version 0.1 July, 2009
   Copyright 2009 Ken Shirriff
   http://arcfn.com
*/
#include <Gamebuino-Meta.h>
#include <IRremote.h>

int RECV_PIN = A3;
int count = 0;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  gb.begin();
  irrecv.enableIRIn(); // Start the receiver
  gb.display.print("INFRARED RECEIVER \n ");
  gb.display.print("\n ");
  gb.display.print("Received message : \n");
  gb.display.print("\n ");

}

void loop() {
  while (!gb.update());
  //gb.display.clear();

  //Clear
  if (count == 2) {
    delay(1000);
    gb.display.clear();
    gb.display.print("INFRARED RECEIVER \n ");
    gb.display.print("\n ");
    gb.display.print("Received message : \n");
    gb.display.print("\n ");
    count = 0;
  }



  //check if a message is received
  if (irrecv.decode(&results)) {

    gb.display.print(results.value, HEX);
    gb.display.print("\n ");
    irrecv.resume(); // Receive the next value
    count = count + 1;
  }
  delay(10);
}
