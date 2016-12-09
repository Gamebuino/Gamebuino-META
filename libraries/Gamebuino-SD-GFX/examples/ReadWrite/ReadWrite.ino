#include <SD.h>
#include <SPI.h>
#include <Adafruit_ZeroDMA.h>
#include <Adafruit_ASFcore.h>
#include <Adafruit_GFX.h>
#include <Image.h>
#include <Adafruit_ST7735.h>
#include <Gamebuino-SD-GFX.h>


#define TFT_CS    (30)
#define TFT_RST   (0)
#define TFT_DC    (31)
#define SD_CS     (PIN_LED_TXL)
#define BTN_CS    (PIN_LED_RXL)
#define NEOPIX    (38)


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

Image image = Image(16, 16, ColorMode::RGB565);

void setup() {
  SerialUSB.begin(9600);
  while (!SerialUSB);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.setColor(BLACK);
  tft.fillScreen();

  if (!SD.begin(SD_CS)) {
    tft.setColor(RED, BLACK);
    tft.println("FAILED");
    delay(1000);
  }
  else {
    tft.setColor(GREEN, BLACK);
    tft.println("OK");
  }

  Gamebuino_SD_GFX::readImage(image, "TEST.BMP");
  SerialUSB.println(image._width);
  SerialUSB.println(image._height);
  for (int i = 0; i < (image._width * image._height); i++) {
    SerialUSB.print(image._buffer[i], HEX);
  }
  tft.drawImage(30, 30, image, 32, 32);

  Gamebuino_SD_GFX::writeImage(image, "WRITE.BMP");
}

void loop() {
  SerialUSB.println("Waiting...");
  delay(1000);
}
