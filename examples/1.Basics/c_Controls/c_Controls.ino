//importe the Gamebuino library and the gb object
#include <Gamebuino-Meta.h>

//declare all the variables needed for the game :
int ball_x = gb.display.width() / 2; //set the horizontal position to the middle of the screen
int ball_y = gb.display.height() / 2; //vertical position
int ball_vx = 1; //horizontal velocity
int ball_vy = 1; //vertical velocity
int ball_size = 6; //the size of the ball in number of pixels

// the setup routine runs once when Gamebuino starts up
void setup() {
  // initialize the Gamebuino
  gb.begin();
}

// the loop routine runs over and over again forever
void loop() {
  // wait until the gamebuino is ready to update at stable 25 FPS
  // this also updates sounds, button presses....everything!
  while (!gb.update());

  // clear the previous screen
  gb.display.clear();

  //move the ball using the buttons
  if (gb.buttons.repeat(BUTTON_RIGHT, 2)) { //every 2 frames when the right button is held down
    ball_x = ball_x + ball_vx; //increase the horizontal position by the ball's velocity
  }
  if (gb.buttons.repeat(BUTTON_LEFT, 2)) {
    ball_x = ball_x - ball_vx;
  }
  if (gb.buttons.repeat(BUTTON_DOWN, 2)) {
    ball_y = ball_y + ball_vy;
  }
  if (gb.buttons.repeat(BUTTON_UP, 2)) {
    ball_y = ball_y - ball_vy;
  }
  //bonus : play a sound when A and B are pressed
  if (gb.buttons.pressed(BUTTON_A)) {
    gb.sound.playOK();
  }
  if (gb.buttons.pressed(BUTTON_B)) {
    gb.sound.playCancel();
  }

  //check that the ball is not going out of the screen
  //if the ball is touching the left side of the screen
  if (ball_x < 0) {
    //bring it back in the screen
    ball_x = 0;
  }
  //if the ball is touching the right side
  if ((ball_x + ball_size) > gb.display.width()) {
    ball_x = gb.display.width() - ball_size;
  }
  //if the ball is touching the top side
  if (ball_y < 0) {
    ball_y = 0;
  }
  //if the ball is touching the down side
  if ((ball_y + ball_size) > gb.display.height()) {
    ball_y = gb.display.height() - ball_size;
  }

  //draw the ball on the screen
  gb.display.fillRect(ball_x, ball_y, ball_size, ball_size);
}