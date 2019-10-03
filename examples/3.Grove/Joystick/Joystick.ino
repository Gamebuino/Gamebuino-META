#include <Gamebuino-Meta.h>

int xAxis = A1;
int yAxis = A2;
int x, y;
int value;

int range = 40;
int offset = 20; // vertical offset


void setup() {
  // put your setup code here, to run once:
  gb.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  while (!gb.update());
  gb.display.clear();

  // reads and maps the value from 0..1023 to -range/2..range/2
  x = analogRead(xAxis) * range / 1024 - (range / 2);
  y = analogRead(yAxis) * range / 1024 - (range / 2);

  //print the values
  gb.display.print("JOYSTICK");
  gb.display.print("\nA1 x:");
  gb.display.print(x);
  gb.display.print("\nA2 y:");
  gb.display.print(y);

  //draw the graphic
  gb.display.drawRect(0, 0 + offset, range, range);
  gb.display.fillRect(0, x + (range / 2) + offset, range, 1);
  gb.display.fillRect(y + range / 2, 0 + offset, 1, range);


}
