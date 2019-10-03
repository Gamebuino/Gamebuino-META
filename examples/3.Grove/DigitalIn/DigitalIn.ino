
#include <Gamebuino-Meta.h>

int MySwitch = A1; //Select the input pin for the switch // Si je mets autre chose que A.. ça ne fonctionne pas
int MyValue;

void setup() {
  // put your setup code here, to run once:
  gb.begin();
  pinMode(MySwitch, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (!gb.update());
  gb.display.clear();

  //update your value from the switch
  MyValue = digitalRead(MySwitch);

  //print it on screen
  gb.display.print("DIGITAL INPUT\nA1: ");

  // If we press the button, "ON" is displayed on screen. Otherwise : "OFF"
  if (MyValue) {
    gb.display.print("ON");
  }
  else {
    gb.display.print("OFF");
  }
}
