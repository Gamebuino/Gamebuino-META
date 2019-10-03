#include <Gamebuino-Meta.h>

int MyPotentiometre = A1; // select the input pin for the potentiometer
int MyValue;

void setup() {
  // put your setup code here, to run once:
  gb.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  while (!gb.update());
  gb.display.clear();

  //update your value from your potentiometer
  MyValue = analogRead(MyPotentiometre);

  //print it on screen
  gb.display.print("ANALOG INPUT\nA1: ");
  gb.display.print(MyValue);

  //maps the value from 0..1023 to 0..64
  MyValue = (MyValue * 64) / 1023;

  //gb.display.print("\n");
  //gb.display.print(MyValue);

  //Draw it as a rectangle
  gb.display.drawRect(0, 16, 64, 5);
  gb.display.fillRect(0, 16, MyValue, 5);
}
