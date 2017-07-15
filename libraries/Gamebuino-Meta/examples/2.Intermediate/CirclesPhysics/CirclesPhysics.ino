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
  if(gb.update()){
    //pause the game if C is pressed
    if(gb.buttons.pressed(BUTTON_C)){
      initGame();
    }
    updateCircles();
    drawCircles();
    
  }
}

void initGame(){
  gb.titleScreen("Circle\ncollision\nphysics");
  gb.pickRandomSeed(); //pick a different random seed each time for games to be different
  initCircles();
}
