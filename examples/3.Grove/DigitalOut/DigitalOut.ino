
#include <Gamebuino-Meta.h>

int MyLed = A1; //Select the input pin for the switch // Si je mets autre chose que A.. ça ne fonctionne pas

void setup() {
  // put your setup code here, to run once:
  gb.begin();
  pinMode(MyLed, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  while (!gb.update());
  gb.display.clear();

  //print it on screen
  gb.display.print("DIGITAL OUTPUT\nA1: ");

  // To enlighten the LED, press A
  if (gb.buttons.repeat(BUTTON_A, 0)) {
    digitalWrite(MyLed, HIGH);
    gb.display.print("ON");
  }
  else {
    digitalWrite(MyLed, LOW);
    gb.display.print("OFF");
  }

}
