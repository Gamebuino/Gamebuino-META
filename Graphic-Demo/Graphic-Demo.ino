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

Image favicon = Image(favicon16Width, favicon16Height, const_cast<uint16_t*>(favicon16));
//Image logoImg = Image(logoWidth, logoHeight, const_cast<uint16_t*>(logo));

Image buffer = Image(80, 64);

void setup(void) {
	tft.initR(INITR_BLACKTAB);
	tft.setRotation(1);
	buffer.drawImage(8, 0, favicon, 64, 64);
	tft.drawImage(0, 0, buffer, tft.width(), tft.height());
	delay(1000);
}

void loop() {
	uint16_t numImages = 64;

	Adafruit_GFX::blendMode = BlendMode::BLEND;
	Adafruit_GFX::alpha = 255;
	drawRandomImages(favicon, numImages);
	Adafruit_GFX::alpha = 255 * 30 / 100;
	drawRandomImages(favicon, numImages);

	Adafruit_GFX::blendMode = BlendMode::ADD;
	drawRandomImages(favicon, numImages);

	Adafruit_GFX::blendMode = BlendMode::SUBSTRACT;
	drawRandomImages(favicon, numImages);

	Adafruit_GFX::blendMode = BlendMode::MULTIPLY;
	drawRandomImages(favicon, numImages);


	Adafruit_GFX::blendMode = BlendMode::SCREEN;
	drawRandomImages(favicon, numImages);
}

void drawRandomImages(Image img, uint16_t num) {
	buffer.fillScreen(tft.Color565(127, 127, 127));
	buffer.setCursor(0, 0);
	switch (Adafruit_GFX::blendMode) {
	case BlendMode::BLEND:
		if (Adafruit_GFX::alpha == 255) {
			buffer.print("TINT");
			break;
		}
		buffer.print("BLEND");
		break;
	case BlendMode::ADD:
		buffer.print("ADD");
		break;
	case BlendMode::SUBSTRACT:
		buffer.print("SUBSTRACT");
		break;
	case BlendMode::MULTIPLY:
		buffer.print("MULTIPLY");
		break;
	case BlendMode::SCREEN:
		buffer.print("SCREEN");
		break;
	}
	for (uint16_t i = 0; i < num; i++) {
		Adafruit_GFX::tint = random(0, 0xFFFF);
		uint16_t scale = random(1, 4);
		buffer.drawImage(
			random(-img.height(), buffer.width()),
			random(10, buffer.height()),
			img, img.width()*scale, img.height()*scale);
		tft.drawImage(0, 0, buffer, tft.width(), tft.height());
	}
}
