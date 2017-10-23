#include <Gamebuino-Meta.h>

extern const byte font3x5[];
extern const byte font5x7[];

//global variables
int camera_x, camera_y;

const SaveDefault savefileDefaults[] = {
  SaveDefault(0, SAVETYPE_INT, 9999),
  SaveDefault(1, SAVETYPE_BLOB, NAMELENGTH+1),
  SaveDefault(2, SAVETYPE_INT, 9999),
  SaveDefault(3, SAVETYPE_BLOB, NAMELENGTH+1),
  SaveDefault(4, SAVETYPE_INT, 9999),
  SaveDefault(5, SAVETYPE_BLOB, NAMELENGTH+1),
  SaveDefault(6, SAVETYPE_INT, 9999),
  SaveDefault(7, SAVETYPE_BLOB, NAMELENGTH+1),
  SaveDefault(8, SAVETYPE_INT, 9999),
  SaveDefault(9, SAVETYPE_BLOB, NAMELENGTH+1),
};

void setup()
{
  gb.begin();
  gb.save.config(savefileDefaults);
  
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
      if(gb.buttons.pressed(BUTTON_C)){
        return;
      }

      updatePlayer();
      updateTime();

      gb.display.setColor(WHITE);
      gb.display.fill();
      gb.display.setColor(BLACK, WHITE);
      drawWorld();
      drawMap();
      drawTime();
      drawPlayer();
    }
  }
}
