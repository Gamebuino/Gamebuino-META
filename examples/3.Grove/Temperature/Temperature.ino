#include <Gamebuino-Meta.h>
#include <math.h>

int thermistor = A1; //Select the input pin for the thermistor
double temperature, B, R0, R, value;


void setup() {
  // put your setup code here, to run once:
  gb.begin();
  pinMode(thermistor, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  while (!gb.update());
  gb.display.clear();

  //read the value of the thermocouple and convert it to temperature
  temperature = 999.99;
  B = 4275.0;  // B value of the thermistor
  R0 = 100000.0;  // R0 = 100k
  value = analogRead(thermistor);
  R = 1024 / value - 1.0; // analog input
  R = R0 * R;
  if ( R0 / R > 0) {
    temperature = 1.0 / ( log(R / R0) / B + 1 / 298.15 ) - 273.15;
  }

  //print it on screen
  gb.display.print("TEMPERATURE SENSOR\nA1: ");
  gb.display.print(value);
  gb.display.print("\nTEMP: ");
  gb.display.print(temperature);
  gb.display.print("C ");


}
