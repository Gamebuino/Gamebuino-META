#include <Gamebuino-Meta.h>

//define de Box structure for obstacles
typedef struct {
  byte w;
  byte x;
  byte h;
  byte y;
} Box;

//define de MovingBox structure for player
typedef struct {
  byte w;
  float x;
  float xv;
  byte h;
  float y;
  float yv;
} MovingBox;

void setup()
{
  gb.begin();
  initGame();
}

void loop(){
  while(!gb.update());
  
  // clear the previous screen
  gb.display.clear();
  
  //pause the game if MENU is pressed
  if(gb.buttons.pressed(BUTTON_MENU)){
    initGame();
  }
  
  updatePlayer();
  updateObstacles();
  
  drawObstacles();
  drawPlayer();
  //gb.display.print(F("CPU:"));
  //gb.display.print(gb.getCpuLoad());
  //gb.display.print(F("%"));
}

void initGame(){
  initObstacles();
  initPlayer();
}
