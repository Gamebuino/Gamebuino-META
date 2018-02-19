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
  
  //pause the game if C is pressed
  if(gb.buttons.pressed(BUTTON_C)){
    initGame();
  }
  updateCircles();
  drawCircles();
}

void initGame(){
  initCircles();
}
