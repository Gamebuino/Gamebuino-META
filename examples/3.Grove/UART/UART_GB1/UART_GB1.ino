#include <Uart.h>
#include <Gamebuino-Meta.h>

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

  //print it on screea
  gb.display.print("UART TEST GB1 \n\n");
  gb.display.print("A to send a msg \n\n");

  if (gb.buttons.repeat(BUTTON_A, 0)) {
    gb.display.print("\nSending Text");
    Serial1.write("\nTest ");

  }

  delay(500);







}
