#include <Gamebuino-Meta.h>

extern const byte font3x5[];
extern const byte font5x7[];

//global variables
int camera_x, camera_y;

void setup()
{
  gb.begin();
  
  initHighscore();
}

void loop(){
  drawTitleScreen();
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
