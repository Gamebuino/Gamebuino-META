#include <Buttons.h>

Buttons buttons;

void setup()
{
  WDT->CTRL.bit.ENABLE = 0;
  buttons.begin();
  SerialUSB.begin(9600);
  
}

void loop()
{
  buttons.update();
  if (buttons.pressed(BTN_DOWN))
    SerialUSB.println("DOWN");
  if (buttons.pressed(BTN_LEFT))
    SerialUSB.println("LEFT");
  if (buttons.pressed(BTN_RIGHT))
    SerialUSB.println("RIGHT");
  if (buttons.pressed(BTN_UP))
    SerialUSB.println("UP");

  if (buttons.pressed(BTN_A))
    SerialUSB.println("A");
  if (buttons.pressed(BTN_B))
    SerialUSB.println("B");
  if (buttons.pressed(BTN_C))
    SerialUSB.println("C");
  if (buttons.pressed(BTN_D))
    SerialUSB.println("D");

  delay(100);
  ((void(*)(const char* filename))(*((uint32_t*)0x3FF8)))("led_red.ino.bin");
}
