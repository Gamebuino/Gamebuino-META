#include <gfxfont.h>
#include <SPI.h>
#include <Adafruit_ZeroDMA.h>
#include <Adafruit_ASFcore.h>
#include <Adafruit_GFX.h>
#include <Image.h>
#include <Adafruit_ST7735.h>
#include <Images/favicon16.h>
//#include <Images/logo.h>


#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

Image favicon = Image(favicon16Width, favicon16Height, const_cast<uint16_t*>(favicon16));
//Image logoImg = Image(logoWidth, logoHeight, const_cast<uint16_t*>(logo));

Image buffer = Image(30, 30);

//up to 16 indexed colors
uint16_t colorIndex[8] = { 0x0000, 0xFFFF, 0xF800, 0xFFE0, 0x07E0, 0x07FF, 0x001F, 0xF81F };

uint16_t imgBuffer[16] = { 0x6116, 0x6226, 0x6336, 0x6446 };
Image image = Image(4, 4, imgBuffer, colorIndex);

//full screen indexed buffer
//160x128x1/2 bytes = 10240B of RAM
Image indexBuffer = Image(160, 128, ColorMode::INDEX, colorIndex);

void setup(void) {
	pinMode(13, OUTPUT);
	tft.initR(INITR_BLACKTAB);
	tft.setRotation(3);
	tft.fillScreen(0x0000);
	buffer.fillScreen(0xFFFF);
	buffer.drawImage(8, 8, image); //draw indexed image to RGB image
	tft.drawImage(0, 0, buffer, buffer.width() * 3, buffer.height() * 3);
	delay(5000);
}

void loop() {
	uint16_t startTime = millis();
	tft.drawImage(0, 0, indexBuffer);
	uint16_t endTime = millis();

	memset(indexBuffer._buffer, 0x11, indexBuffer.width()*indexBuffer.height() / 2); //clear buffer
	indexBuffer.drawCircle((millis() / 10) % 160, 20, 10, 2); //draw to indexed buffer
	indexBuffer.setTextColor(6);
	indexBuffer.setCursor(10, 10);
	indexBuffer.print(endTime - startTime);
	indexBuffer.println("ms");
	indexBuffer.setTextColor(7);
	indexBuffer.setCursor(64, 10);
	indexBuffer.print(1000 / (endTime - startTime));
	indexBuffer.println("FPS");
}