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
  gb.display.setFont(font5x7);
  initHighscore();
  drawTitleScreen();
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
        gb.display.setFont(font5x7);
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


