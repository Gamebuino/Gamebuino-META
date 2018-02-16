/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Graphics.h>    // Core graphics library
#include <Display_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS    30
#define TFT_RST   0
#define TFT_DC    31

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Display_ST7735 tft = Display_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Display_ST7735 tft = Display_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


float p = 3.1415926;

void setup(void) {
  //Serial.begin(9600);
  //Serial.print("Hello! ST7735 TFT Test");

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  //tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

  //Serial.println("Initialized");

  uint16_t time = millis();
  tft.setColor(BLACK);
  tft.fillScreen();
  time = millis() - time;

  //Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.setColor(BLACK);
  tft.fillScreen();
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", WHITE);
  delay(1000);

  // tft print function!
  tftPrintTest();
  delay(4000);

  // a single pixel
  tft.setColor(GREEN);
  tft.drawPixel(tft.width()/2, tft.height()/2);
  delay(500);

  // line draw test
  testlines(YELLOW);
  delay(500);

  // optimized lines
  testfastlines(RED, BLUE);
  delay(500);

  testdrawrects(GREEN);
  delay(500);

  testfillrects(YELLOW, MAGENTA);
  delay(500);
  
  tft.setColor(BLACK);
  tft.fillScreen();
  testfillcircles(10, BLUE);
  testdrawcircles(10, WHITE);
  delay(500);

  testroundrects();
  delay(500);

  testtriangles();
  delay(500);

  mediabuttons();
  delay(500);

  //Serial.println("done");
  delay(1000);
}

void loop() {
  tft.invertDisplay(true);
  delay(500);
  tft.invertDisplay(false);
  delay(500);
}

void testlines(uint16_t color) {
  tft.setColor(BLACK);
  tft.fillScreen();
  tft.setColor(color);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y);
  }
  
  tft.setColor(BLACK);
  tft.fillScreen();
  
  tft.setColor(color);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y);
  }

  tft.setColor(BLACK);
  tft.fillScreen();
  
  tft.setColor(color);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y);
  }

  tft.setColor(BLACK);
  tft.fillScreen();
  
  tft.setColor(color);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.setColor(BLACK);
  tft.fillScreen();
  
  tft.setColor(color1);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width());
  }
  tft.setColor(color2);
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height());
  }
}

void testdrawrects(uint16_t color) {
  tft.setColor(BLACK);
  tft.fillScreen();
  tft.setColor(color);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.setColor(BLACK);
  tft.fillScreen();
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
  tft.setColor(color1);
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x);
  tft.setColor(color2);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  tft.setColor(color);
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  tft.setColor(color);
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius);
    }
  }
}

void testtriangles() {
  tft.setColor(BLACK);
  tft.fillScreen();
  int color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  tft.setColor(color);
  for(t = 0 ; t <= 15; t+=1) {
    tft.drawTriangle(w, y, y, x, z, x);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.setColor(BLACK);
  tft.fillScreen();
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    tft.setColor(color);
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.setColor(WHITE);
  tft.fillScreen();
  tft.setCursor(0, 30);
  tft.setColor(RED);
  tft.setFontSize(1);
  tft.println("Hello World!");
  tft.setColor(YELLOW);
  tft.setFontSize(2);
  tft.println("Hello World!");
  tft.setColor(GREEN);
  tft.setFontSize(3);
  tft.println("Hello World!");
  tft.setColor(BLUE);
  tft.setFontSize(4);
  tft.print(1234.567);
  delay(1500);
  
  tft.setCursor(0, 0);
  tft.setColor(BLACK);
  tft.fillScreen();
  tft.setColor(WHITE);
  tft.setFontSize(0);
  tft.println("Hello World!");
  tft.setFontSize(1);
  tft.setColor(GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setColor(WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setColor(MAGENTA);
  tft.print(millis() / 1000);
  tft.setColor(WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  /*tft.fillScreen(BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, GREEN);*/
}
