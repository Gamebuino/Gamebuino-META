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

Image buffer = Image(80, 64);

void setup(void) {
	pinMode(13, OUTPUT);
	tft.initR(INITR_BLACKTAB);
	tft.setRotation(3);
	tft.fillScreen(0x0000);
	buffer.drawImage(8, 0, favicon, 64, 64);
	tft.drawImage(0, 0, buffer, tft.width(), tft.height());
	delay(1000);
}

void loop() {
	while (millis() < 5000) {
		uint16_t startTime = millis();
		PORT->Group[0].OUTSET.reg = (1 << 17);  // set PORTA.17 high  "digitalWrite(13, HIGH)"
		tft.drawImage(0, 0, buffer, buffer.width(), buffer.height());
		PORT->Group[0].OUTCLR.reg = (1 << 17);  // clear PORTA.17 high "digitalWrite(13, LOW)"
		uint16_t endTime = millis();

		buffer.fillScreen(0xFFFF);
		buffer.setTextColor(0x0000);
		buffer.setCursor(0, 0);

		buffer.print(endTime - startTime);
		buffer.println("ms");
		buffer.print(1000 / (endTime - startTime));
		buffer.println("FPS");
		buffer.drawImage((millis() / 10) % (buffer.width() - 16), 32, favicon);
	}

	while (1) {
		uint16_t startTime = millis();
		PORT->Group[0].OUTSET.reg = (1 << 17);  // set PORTA.17 high  "digitalWrite(13, HIGH)"
		tft.drawImage(0, 0, buffer, tft.width(), tft.height());
		PORT->Group[0].OUTCLR.reg = (1 << 17);  // clear PORTA.17 high "digitalWrite(13, LOW)"
		uint16_t endTime = millis();

		buffer.fillScreen(0xFFFF);
		buffer.setTextColor(0x0000);

		buffer.setCursor(10, 10);
		buffer.print(endTime - startTime);
		buffer.println("ms");
		buffer.setCursor(64, 10);
		buffer.print(1000 / (endTime - startTime));
		buffer.println("FPS");
		buffer.drawImage((millis() / 10) % (buffer.width()-16) , 32, favicon);
	}
}

