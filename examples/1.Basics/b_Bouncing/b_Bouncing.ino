//imports the Gamebuino library and the gb object
#include <Gamebuino-Meta.h>

//declare all the variables needed for the game :
int ball_x = gb.display.width() / 2; //set the horizontal position to the middle of the screen
int ball_y = gb.display.height() / 2; //vertical position
int ball_vx = 1; //horizontal velocity
int ball_vy = 1; //vertical velocity
const int ball_size = 6; //the size of the ball in number of pixels

// the setup routine runs once when Gamebuino starts up
void setup() {
  // initialize the Gamebuino object
  gb.begin();
}

// the loop routine runs over and over again forever
void loop() {
  // wait until the gamebuino is ready to update at stable 25 FPS
  // this also updates sounds, button presses....everything!
  gb.waitForUpdate();

  // clear the previous screen
  gb.display.clear();

  //add the speed of the ball to its position
  ball_x = ball_x + ball_vx;
  ball_y = ball_y + ball_vy;
  
  //check that the ball is not going out of the screen
  if (ball_x <= 0   //if the ball is touching the left side of the screen
  || (ball_x + ball_size) >= gb.display.width()) {  //OR if the ball is touching the right side
    ball_vx = -ball_vx; //change the direction of the horizontal speed
    gb.sound.playTick();//play a preset "tick" sound when the ball hits the border
  }

  if (ball_y <= 0   //if the ball is touching the top side
  || (ball_y + ball_size) >= gb.display.height()) { //OR if the ball is touching the down side
    ball_vy = -ball_vy; //change the direction of the vertical speed
    gb.sound.playTick();
  }

  //draw the ball on the screen
  gb.display.fillRect(ball_x, ball_y, ball_size, ball_size);
}