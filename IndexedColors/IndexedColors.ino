#include <gfxfont.h>
#include <SPI.h>
#include <Adafruit_ZeroDMA.h>
#include <Adafruit_ASFcore.h>
#include <Adafruit_GFX.h>
#include <Image.h>
#include <Adafruit_ST7735.h>
#include <Images/favicon16.h>
//#include <Images/logo.h>

#define TFT_CS		(30)
#define TFT_RST		(0)
#define TFT_DC		(31)
#define SD_CS		(PIN_LED_TXL)
#define BTN_CS		(PIN_LED_RXL)
#define NEOPIX		(38)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

Image favicon = Image(favicon16Width, favicon16Height, ColorMode::RGB565, const_cast<uint16_t*>(favicon16));
//Image logoImg = Image(logoWidth, logoHeight, const_cast<uint16_t*>(logo));

Image buffer = Image(30, 30, ColorMode::RGB565);

//up to 16 indexed colors
/*uint16_t colorIndex[16] = { 0x0,
0x194A,
0x792A,
0x42A,
0xAA86,
0x62C9,
0xC618,
0xFF9D,
0xE8CA,
0xFD03,
0xF765,
0x5DC9,
0x553B,
0x83B3,
0xF3B4,
0xFE75,
};*/

uint16_t imgBuffer[16] = { 0x6116, 0x6226, 0x6336, 0x6446 };
Image image = Image(4, 4, ColorMode::INDEX, imgBuffer);

//full screen indexed buffer
//160x128x1/2 bytes = 10240B of RAM
Image indexBuffer = Image(160, 128, ColorMode::INDEX);

void setup(void) {
	pinMode(13, OUTPUT);
	tft.initR(INITR_BLACKTAB);
	tft.setRotation(1);
	tft.fillScreen(0x0000);
	buffer.fillScreen(0xFFFF);
	buffer.drawImage(8, 8, image); //draw indexed image to RGB image
	tft.drawImage(0, 0, buffer, buffer.width() * 3, buffer.height() * 3);
	delay(2000);
}

void loop() {
	uint16_t startTime = millis();
	tft.drawImage(0, 0, indexBuffer);
	uint16_t endTime = millis();

	memset(indexBuffer._buffer, 0x11, indexBuffer.width()*indexBuffer.height() / 2); //clear buffer
	indexBuffer.drawCircle((millis() / 10) % 160, 20, 10, 13); //draw to indexed buffer
	indexBuffer.setCursor(0, 48);
	for (uint16_t i = 0; i < 16; i++) {
		indexBuffer.fillRect(8+i*9, 32, 8, 8, i);
		indexBuffer.setTextColor(1,i);
		if (i == 1) indexBuffer.setTextColor(0, i);
		if (i < 10) indexBuffer.print(" ");
		indexBuffer.print(i);
	}
	indexBuffer.setTextColor(3);
	indexBuffer.setCursor(10, 10);
	indexBuffer.print(endTime - startTime);
	indexBuffer.println("ms");
	indexBuffer.setTextColor(11);
	indexBuffer.setCursor(64, 10);
	indexBuffer.print(1000 / (endTime - startTime));
	indexBuffer.println("FPS");
}