
#include <Gamebuino-Meta.h>
#include <Uart.h>

char c;

void setup() {
  // put your setup code here, to run once:
  gb.begin();
  Serial1.begin(9600);


}

void loop() {
  // put your main code here, to run repeatedly:
  while (!gb.update());
  gb.display.clear();

  // Wait a text from GB1
  gb.display.print("UART TEST GB2\n\n");
  gb.display.print("A text from GB1 :\n\n");


  //Receive the text and print it
  while (Serial1.available()) {
    c = Serial1.read();
    gb.display.print(c);
  }


  delay(1000);


}
