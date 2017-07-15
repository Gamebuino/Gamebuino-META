#include <Gamebuino-Meta.h>

void setup()
{
  gb.begin();
}

void loop()
{
  if (gb.update()) {
    gb.display.println("INDEX 160x128");
    uint16_t startTime = millis();
    PORT->Group[0].OUTSET.reg = (1 << 17);  // set PORTA.17 high  "digitalWrite(13, HIGH)"
    gb.tft.drawImage(0, 0, gb.display, gb.tft.width(), gb.tft.height());
    PORT->Group[0].OUTCLR.reg = (1 << 17);  // clear PORTA.17 high "digitalWrite(13, LOW)"
    uint16_t endTime = millis();
    gb.display.print(endTime - startTime);
    gb.display.println("ms");
    gb.display.print(1000 / (endTime - startTime));
    gb.display.println("FPS");
  }
}