#include <Gamebuino-Meta.h>


Image image = Image(16, 16, ColorMode::rgb565);
Image imageIndex = Image(16, 16, ColorMode::index);

void setup() {
  gb.begin();
  SerialUSB.begin(9600);
  //while (!SerialUSB);

  Gamebuino_Meta::Gamebuino_SD_GFX::debugOutput = &gb.tft;
  
/*
  Gamebuino_Meta::Gamebuino_SD_GFX::readImage(image, "RGB888.BMP");
  SerialUSB.println(image._width);
  SerialUSB.println(image._height);
  for (int i = 0; i < (image._width * image._height); i++) {
    SerialUSB.print(image._buffer[i], HEX);
  }
  gb.tft.drawImage(120, 8, image, 32, 32);

  Gamebuino_Meta::Gamebuino_SD_GFX::writeImage(image, "WRITE888.BMP");

  delay(3000);
  gb.tft.setColor(BLACK);
  gb.tft.fillScreen();
  gb.tft.setColor(WHITE, BLACK);
  gb.tft.setCursor(0,0);
*/
  //INDEX4 BITMAP

  Gamebuino_Meta::Gamebuino_SD_GFX::readImage(imageIndex, "INDEX4.BMP");
//  SerialUSB.println(imageIndex._width);
//  SerialUSB.println(imageIndex._height);
//  gb.tft.drawImage(120, 8, imageIndex, 32, 32);

//  Gamebuino_Meta::Gamebuino_SD_GFX::writeImage(imageIndex, "WRITE4.BMP");
  
  Image screenIndex = Image(160, 128, ColorMode::index);
  screenIndex.drawImage(0, LCDHEIGHT*2 - 16, imageIndex);
  screenIndex.drawImage(0, LCDHEIGHT*2 - 15, imageIndex);
  gb.tft.drawImage(0, 0, screenIndex);
  //delay(3000);
  /*
  gb.tft.setColor(BLACK);
  gb.tft.fillScreen();
  gb.tft.setColor(WHITE, BLACK);
  gb.tft.setCursor(0,0);
  for (int i = 0; i < ((imageIndex._width + 1) * imageIndex._height / 2); i++) {
    if(!(i%((imageIndex._width + 1)/2))){
      gb.tft.print("\n");
    }
    gb.tft.print((((uint8_t*)imageIndex._buffer)[i]>>4)&0x0F, HEX);
    gb.tft.print((((uint8_t*)imageIndex._buffer)[i])&0x0F, HEX);
  }
  */
}

void loop() {
  
}
