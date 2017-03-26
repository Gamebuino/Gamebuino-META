#include <SPI.h>
#include <Gamebuino.h>
#include <Gamebuino-SD-GFX.h>
Gamebuino gb;

//#include <EEPROM.h>
#include <avr/pgmspace.h>

extern const byte font3x5[];
extern const byte font5x7[];

//global variables
int camera_x, camera_y;

void setup()
{
  gb.begin();
  gb.tft.println(gb.save.get(0));
  gb.tft.println(gb.save.get(1));
  
  
  //gb.save.set(2, "Hello World?");
  //gb.save.set(3, "Foxes are BEST!!!!!");
  
  gb.tft.println(gb.save.get(0));
  gb.tft.println(gb.save.get(1));
  char buf[32];
  gb.save.get(2, buf, 32);
  gb.tft.println(buf);
  gb.save.get(3, buf, 32);
  gb.tft.println(buf);
  
  //gb.save.set(15, "yay");
  
  gb.save.get(15, buf, 32);
  gb.tft.println(buf);
  while(1);
  initHighscore();
}

void loop(){
  drawMenu();
}

void initGame(){
  //gb.battery.show = false;
  initPlayer();
  initTime();
}

void play(){
  gb.display.setFont(font3x5);
  while(1){
    if(gb.update()){
      //pause the game if C is pressed
      if(gb.buttons.pressed(BTN_C)){
        return;
      }

      updatePlayer();
      updateTime();

      drawWorld();
      drawMap();
      drawTime();
      drawPlayer();
    }
  }
}
