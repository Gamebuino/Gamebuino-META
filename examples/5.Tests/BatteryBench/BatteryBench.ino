#include <Gamebuino-Meta.h>

void setup() {
  gb.begin();

  gb.tft.print("OPENING FILE... ");
  //open the file (or create if not existing), read/write mode, start writing at the end.
  File file = SD.open("BATTERY.TXT", O_CREAT | O_RDWR | O_AT_END);
  if (!file) {
    gb.tft.println("ERROR");
    while (1);
  } else {
    gb.tft.println("OK");
    file.print("\n### STARTING BATTERY BENCHMARK ###\n");
    file.flush();
  }

  while (1) {
    while(!gb.update());
    gb.display.clear();
    gb.lights.clear();
    gb.display.println("BATTERY BENCHMARK\n");
    gb.display.println("I'VE BEEN UP FOR ");
    gb.display.print(millis() / 1000 / 60 / 60 );
    gb.display.println(" HOURS ");
    gb.display.print((millis() / 1000 / 60) % 60);
    gb.display.println(" MINUTES ");
    gb.display.print((millis() / 1000) % 60);
    gb.display.println(" SECONDS ");
    gb.display.print(millis() % 1000);
    gb.display.println(" MILLISEC ");
    if ((gb.frameCount % (25 * 60)) == 0) {
      file.print(millis() / 1000 / 60 / 60 );
      file.print("H ");
      file.print((millis() / 1000 / 60) % 60);
      file.print("M ");
      file.print((millis() / 1000) % 60);
      file.print("S (");
      file.print(millis());
      file.println("ms)");
      file.flush();
    }
  }
}

void loop() {
}
