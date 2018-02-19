#include <Gamebuino-Meta.h>

void screenBenchmark();
void colorTest1();
int mode = 0;
#define NUMMODES 3

unsigned long lastMillis = 0;
boolean paused = false;

void setup() {
  gb.begin();
}

//dynamicaly change between display modes
//this will jst break after looping for a while,
//probably because of memory fragmentation
void loop() {
  screenBenchmark();
  colors();
  donothing();
}

void drawBottomLine() {
  //Current buffer mode
  gb.display.cursorX = 0;
  gb.display.cursorY = gb.display.height() - 11;
  gb.display.setColor(WHITE);
  gb.display.print(gb.display.width());
  gb.display.print("x");
  gb.display.print(gb.display.height());
  if (gb.display.colorMode == ColorMode::rgb565) {
    gb.display.print(" RGB");
  } else {
    gb.display.print(" INDEX");
  }
  gb.display.setColor(BROWN);
  gb.display.cursorX = 13*4;
  gb.display.setColor(BLUE);
  gb.display.print(" >");
  gb.display.setColor(BROWN);
  gb.display.print("MODE ");

  //bottom line indications
  gb.display.setColor(BROWN);
  gb.display.cursorX = 0;
  gb.display.cursorY = gb.display.height() - 5;
  gb.display.setColor(GREEN);
  gb.display.print("A");
  gb.display.setColor(BROWN);
  gb.display.print("SKIP");
  gb.display.setColor(RED);
  gb.display.print(" B");
  gb.display.setColor(BROWN);
  (paused == true) ? gb.display.print("PLAY ") : gb.display.print("STOP ");
  gb.display.print(gb.getCpuLoad());
  gb.display.print("%");
  gb.display.print(" ");
  gb.display.print(gb.frameCount%100);
}

boolean skip() {
  if (gb.buttons.pressed(BUTTON_RIGHT)) {
    mode++;
    if (mode >= NUMMODES) {
      mode = 0;
    }
    switch (mode) {
      case 0:
        gb.display.init(80, 64, ColorMode::rgb565);
        break;
      case 1:
        gb.display.init(160, 128, ColorMode::index);
        break;
      case 2:
        gb.display.init(80, 64, ColorMode::index);
        break;
    }
  }


  drawBottomLine();

  //pause/unpause when B is pressed
  if (gb.buttons.pressed(BUTTON_B)) {
    paused = !paused;
  }
  //skip frame when A is pressed
  if (gb.buttons.pressed(BUTTON_A)) {
    paused = true;
    return true;
  }
  //skip every 2 seconds if not paused
  if (((millis() - lastMillis) > 4000) && !paused) {
    lastMillis = millis();
    return true;
  }
  return false;
}

void donothing() {
  while (1) {
    while(!gb.update());
    // clear the previous screen
    gb.display.clear();
    if (skip()) {
      return;
    }
  }
}

void screenBenchmark() {
  //Screen update benchmark
  unsigned long timeBenchmark = 20;
  while (1) {
    while(!gb.update());
    // clear the previous screen
    gb.display.clear();
    
    gb.display.setColor(WHITE);
    gb.display.print(1000 / (timeBenchmark));
    gb.display.print(" FPS");
    gb.display.setColor(BROWN);
    gb.display.print(" (");
    gb.display.print(timeBenchmark);
    gb.display.println("ms)");
    gb.display.setColor(WHITE);
    gb.display.print(gb.display.getBufferSize() / 1024);
    gb.display.print(" KB");
    gb.display.setColor(BROWN);
    gb.display.println(" BUFFER");

    drawBottomLine();
    uint16_t startTime = millis();
    PORT->Group[0].OUTSET.reg = (1 << 17);  // set PORTA.17 high  "digitalWrite(13, HIGH)"
    gb.tft.drawImage(0, 0, gb.display, gb.tft.width(), gb.tft.height());
    PORT->Group[0].OUTCLR.reg = (1 << 17);  // clear PORTA.17 high "digitalWrite(13, LOW)"
    uint16_t endTime = millis();
    timeBenchmark = endTime - startTime;
    if (skip()) {
      return;
    }
  }
}

void colors() {
  //Colors test 1/2
  while (1) {
    while(!gb.update());
    // clear the previous screen
    gb.display.clear();
    
    //first column
    gb.display.setColor(WHITE);
    gb.display.println("WHITE");
    gb.display.setColor(GRAY);
    gb.display.println("GRAY");
    gb.display.setColor(DARKGRAY);
    gb.display.println("DARKGRAY");
    gb.display.setColor(BLACK, DARKGRAY);
    gb.display.println("BLACK");
    gb.display.setColor(PURPLE);
    gb.display.println("PURPLE");
    gb.display.setColor(PINK);
    gb.display.println("PINK");
    gb.display.setColor(RED);
    gb.display.println("RED");
    gb.display.setColor(ORANGE);
    gb.display.println("ORANGE");

    //second column
    gb.display.cursorY = 0;
    gb.display.cursorX = 36;
    gb.display.setColor(BROWN);
    gb.display.println("BROWN");
    gb.display.cursorX = 36;
    gb.display.setColor(BEIGE);
    gb.display.println("BEIGE");
    gb.display.cursorX = 36;
    gb.display.setColor(YELLOW);
    gb.display.println("YELLOW");
    gb.display.cursorX = 36;
    gb.display.setColor(LIGHTGREEN);
    gb.display.println("LIGHTGREEN");
    gb.display.cursorX = 36;
    gb.display.setColor(GREEN);
    gb.display.println("GREEN");
    gb.display.cursorX = 36;
    gb.display.setColor(DARKBLUE);
    gb.display.println("DARKBLUE");
    gb.display.cursorX = 36;
    gb.display.setColor(BLUE);
    gb.display.println("BLUE");
    gb.display.cursorX = 36;
    gb.display.setColor(LIGHTBLUE);
    gb.display.println("LIGHTBLUE");
    if (skip()) {
      return;
    }
  }
}
