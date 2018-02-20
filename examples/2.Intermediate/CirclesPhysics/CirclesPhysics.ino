#include <Gamebuino-Meta.h>

typedef struct{
  float x, y, r, vx, vy;
} Circle;

void setup()
{
  gb.begin();
  initGame();
}

void loop()
{
  while(!gb.update());
  
  // clear the previous screen
  gb.display.clear();
  
  //pause the game if MENU is pressed
  if(gb.buttons.pressed(BUTTON_MENU)){
    initGame();
  }
  updateCircles();
  drawCircles();
}

void initGame(){
  initCircles();
}
