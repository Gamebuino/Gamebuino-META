#include <SPI.h>
#define BTN_CS    PIN_LED_RXL

//HC165 input expander
//Data is shifted on HIGH to LOW (falling) clock edge
//So we read SPI value on LOW to HIGH (rising) clock edge

//Mode / Clock Polarity (CPOL) / Clock Phase (CPHA) / Output Edge / Data Capture
//SPI_MODE0 0 0 Falling Rising
//SPI_MODE1 0 1 Rising  Falling
//SPI_MODE2 1 0 Rising  Falling
//SPI_MODE3 1 1 Falling Rising

void setup() {
  SerialUSB.begin(9600);
  SPI.begin();
  pinMode(BTN_CS, OUTPUT);
  digitalWrite(BTN_CS, HIGH);
}

void loop() {
  SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));
  digitalWrite(BTN_CS, LOW);
  delayMicroseconds(1);
  byte buttonsData = SPI.transfer(1);
  digitalWrite(BTN_CS, HIGH);
  SPI.endTransaction();
  SerialUSB.println(buttonsData,BIN);
  delay(50);
}
