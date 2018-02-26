#include <Gamebuino-Meta.h>

boolean paused = false;

//player variables
int player_score = 0;
int player_h = 16;
int player_w = 3;
int player_x = 0;
int player_y = (gb.display.height() - player_h) / 2;
int player_vy = 2;

//oponent variables
int oponent_score = 0;
int oponent_h = 16;
int oponent_w = 3;
int oponent_x = gb.display.width() - oponent_w;
int oponent_y = (gb.display.height() - oponent_h) / 2;
int oponent_vy = 2;

//ball variables
int ball_size = 6;
int ball_x = gb.display.width() - ball_size - oponent_w - 1;
int ball_y = (gb.display.height() - ball_size) / 2;
int ball_vx = 3;
int ball_vy = 3;

extern const byte font5x7[]; //get the default large font

///////////////////////////////////// SETUP
void setup() {
  gb.begin();
}

///////////////////////////////////// LOOP
void loop() {
  while (!gb.update());

  // clear the previous screen
  gb.display.clear();

  //move the player
  if (gb.buttons.repeat(BUTTON_UP, 1)) {
    player_y = max(-player_y, player_y - player_vy);
  }
  if (gb.buttons.repeat(BUTTON_DOWN, 1)) {
    player_y = min(gb.display.height(), player_y + player_vy);
  }

  //move the ball
  ball_x = ball_x + ball_vx;
  ball_y = ball_y + ball_vy;

  //check for ball collisions
  //collision with the top border
  if (ball_y < 0) {
    ball_y = 0;
    ball_vy = -ball_vy;
  }
  //collision with the bottom border
  if ((ball_y + ball_size) > gb.display.height()) {
    ball_y = gb.display.height() - ball_size;
    ball_vy = -ball_vy;
  }
  //collision with the player
  if (gb.collideRectRect(ball_x, ball_y, ball_size, ball_size, player_x, player_y, player_w, player_h)) {
    ball_x = player_x + player_w;
    ball_vx = -ball_vx;
  }
  //collision with the oponent
  if (gb.collideRectRect(ball_x, ball_y, ball_size, ball_size, oponent_x, oponent_y, oponent_w, oponent_h)) {
    ball_x = oponent_x - ball_size;
    ball_vx = -ball_vx;
  }
  //collision with the left side
  if (ball_x < 0) {
    oponent_score = oponent_score + 1;
    ball_x = gb.display.width() - ball_size - oponent_w - 1;
    ball_vx = -abs(ball_vx);
    ball_y = random(0, gb.display.height() - ball_size);
  }
  //collision with the right side
  if ((ball_x + ball_size) > gb.display.width()) {
    player_score = player_score + 1;
    ball_x = gb.display.width() - ball_size - oponent_w - 16; //place the ball on the oponent side
    ball_vx = -abs(ball_vx);
    ball_y = random(0, gb.display.height() - ball_size);

  }
  //reset score when 10 is reached
  if ((player_score == 10) || (oponent_score == 10)) {
    player_score = 0;
    oponent_score = 0;
  }

  //move the oponent
  if ((oponent_y + (oponent_h / 2)) < (ball_y + (ball_size / 2))) { //if the ball is below the oponent
    oponent_y = oponent_y + oponent_vy; //move down
    oponent_y = min(gb.display.height() - oponent_h, oponent_y); //don't go out of the screen
  }
  else {
    oponent_y = oponent_y - oponent_vy; //move up
    oponent_y = max(0, oponent_y); //don't go out of the screen
  }


  //draw the score
  //gb.display.fontSize = 2;
  gb.display.setCursor(15, 16);
  gb.display.print(player_score);

  gb.display.setCursor(57, 16);
  gb.display.print(oponent_score);

  //draw the ball
  gb.display.fillRect(ball_x, ball_y, ball_size, ball_size);
  //draw the player
  gb.display.fillRect(player_x, player_y, player_w, player_h);
  //draw the oponent
  gb.display.fillRect(oponent_x, oponent_y, oponent_w, oponent_h);
}