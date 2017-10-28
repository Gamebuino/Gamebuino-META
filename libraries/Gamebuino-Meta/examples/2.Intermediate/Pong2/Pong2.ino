#include <Gamebuino-Meta.h>

boolean paused = false;

//player variables
int player_score = 0;
int player_h = 8;
int player_w = 2;
int player_x = 4;
int player_y = (gb.display.height() - player_h) / 2;
int player_vymax = 4;
int player_vy = 0;
int player_timeoutmax = 80;
int player_timeout = player_timeoutmax;

//oponent variables
int oponent_score = 0;
int oponent_h = 8;
int oponent_w = 2;
int oponent_x = gb.display.width() - oponent_w - 4;
int oponent_y = (gb.display.height() - oponent_h) / 2;
int oponent_vymax = 4;
int oponent_vy = 0;
int oponent_timeoutmax = 80;
int oponent_timeout = oponent_timeoutmax;

//ball variables
int ball_size = 3;
int ball_x = oponent_x - ball_size - 5;
int ball_y = (gb.display.height() - ball_size) / 2;
int ball_vx = 2;
int ball_vy = 0;
int ball_vymax = 6;

extern const byte font5x7[]; //get the default large font

///////////////////////////////////// SETUP
void setup() {
  gb.begin();
  //gb.display.setFont(font5x7); //change the font to the large one
  //gb.titleScreen("Pong Solo");
  gb.pickRandomSeed(); //pick a different random seed each time for games to be different
  //gb.battery.show = false; //hide the battery indicator
}

///////////////////////////////////// LOOP
void loop() {
  if (gb.update()) {
    // clear the previous screen
    gb.display.clear();
    gb.lights.clear();


    //pause the game if C is pressed
    if (gb.buttons.pressed(BUTTON_C)) {
      //gb.titleScreen("Pong Solo");
      //gb.battery.show = false;
      //gb.display.fontSize = 2;
      delay(500);
    }
    //move the player
    if (gb.buttons.repeat(BUTTON_UP, 1)) {
      player_timeout = player_timeoutmax;
      player_vy -= 1;
    } else {
      if (gb.buttons.repeat(BUTTON_DOWN, 1)) {
        player_timeout = player_timeoutmax;
        player_vy += 1;
      } else {
        if (player_timeout > 0) {
          player_vy = player_vy / 2;
        }
      }
    }
    //computer control after timeout
    if (player_timeout > 0) {
      player_timeout--;
    } else {
      if ((player_y + (player_h / 2)) < (ball_y + (ball_size / 2))) { //if the ball is below the oponent
        player_vy = player_vy + 1; //move down
      }
      else {
        player_vy = player_vy - 1; //move up
      }
    }
    player_vy = constrain(player_vy, (-player_vymax), player_vymax);
    player_y = player_y + player_vy;
    player_y = constrain(player_y, (-player_h), gb.display.height());


    //move the oponent
    //human control
    if (gb.buttons.repeat(BUTTON_B, 1)) {
      oponent_timeout = oponent_timeoutmax;
      oponent_vy -= 1;
      oponent_vy = constrain(oponent_vy, (-oponent_vymax), oponent_vymax);
    } else {
      if (gb.buttons.repeat(BUTTON_A, 1)) {
        oponent_timeout = oponent_timeoutmax;
        oponent_vy += 1;
        oponent_vy = constrain(oponent_vy, (-oponent_vymax), oponent_vymax);
      } else {
        if (oponent_timeout > 0) {
          oponent_vy = oponent_vy / 2;
        }
      }
    }
    //computer control after timeout
    if (oponent_timeout > 0) {
      oponent_timeout--;
    } else {
      if ((oponent_y + (oponent_h / 2)) < (ball_y + (ball_size / 2))) { //if the ball is below the oponent
        oponent_vy = oponent_vy + 1; //move down
      }
      else {
        oponent_vy = oponent_vy - 1; //move up
      }
    }
    oponent_vy = constrain(oponent_vy, (-oponent_vymax), oponent_vymax);
    oponent_y = oponent_y + oponent_vy;
    oponent_y = constrain(oponent_y, (-oponent_h), gb.display.height()); //don't go out of the screen

    //move the ball
    ball_x = ball_x + ball_vx;
    ball_y = ball_y + ball_vy;

    //check for ball collisions
    //collision with the top border
    if (ball_y < 0) {
      ball_y = 0;
      ball_vy = -ball_vy;
      gb.sound.playTick();
    }
    //collision with the bottom border
    if ((ball_y + ball_size) > gb.display.height()) {
      ball_y = gb.display.height() - ball_size;
      ball_vy = -ball_vy;
      gb.sound.playTick();
    }
    //collision with the player
    if (gb.collideRectRect(ball_x, ball_y, ball_size, ball_size, player_x, player_y, player_w, player_h)) {
      ball_x = player_x + player_w;
      ball_vx = -ball_vx;
      ball_vy += player_vy / 2; //add some effet to the ball
      ball_vy = constrain(ball_vy, (-ball_vymax), ball_vymax);
      gb.sound.playTick();
      gb.lights.drawPixel(0, map(ball_y, 0, gb.display.height(), 0, 4));
    }
    //collision with the oponent
    if (gb.collideRectRect(ball_x, ball_y, ball_size, ball_size, oponent_x, oponent_y, oponent_w, oponent_h)) {
      ball_x = oponent_x - ball_size;
      ball_vx = -ball_vx;
      ball_vy += (oponent_vy / 2);
      gb.sound.playTick();
      gb.lights.drawPixel(1, map(ball_y, 0, gb.display.height(), 0, 4));
    }
    //collision with the left side
    if (ball_x < 0) {
      oponent_score = oponent_score + 1;
      gb.sound.playCancel();
      gb.lights.setColor(RED);
      gb.lights.drawPixel(0, map(ball_y, 0, gb.display.height(), 0, 4));
      player_y = (gb.display.height() / 2) - (player_h / 2);
      oponent_y = player_y;
      ball_x = oponent_x - 4;
      ball_vx = -abs(ball_vx);
      ball_vy = 0;
      ball_y = random(0, gb.display.height() - ball_size);
    }
    //collision with the right side
    if ((ball_x + ball_size) > gb.display.width()) {
      player_score = player_score + 1;
      gb.sound.playOK();
      gb.lights.setColor(RED);
      gb.lights.drawPixel(1, map(ball_y, 0, gb.display.height(), 0, 4));
      player_y = (gb.display.height() / 2) - (player_h / 2);
      oponent_y = player_y;
      ball_x = player_x + player_w + 5;
      ball_vx = abs(ball_vx);
      ball_vy = 0;
      ball_y = random(0, gb.display.height() - ball_size);

    }
    //reset score when 10 is reached
    if ((player_score == 10) || (oponent_score == 10)) {
      player_score = 0;
      oponent_score = 0;
    }

    gb.display.setColor(WHITE);
    //draw the score
    //gb.display.fontSize = 2;
    gb.display.setCursor(15, 16);
    gb.display.print(player_score);

    gb.display.setCursor(gb.display.width() - 3 - 15, 16);
    gb.display.print(oponent_score);

    //draw the middle line
    for (int i = 0; i < gb.display.height(); i += 4) {
      gb.display.drawRect(gb.display.width() / 2, i, 1, 2);
    }
    //draw the ball
    gb.display.fillRect(ball_x, ball_y, ball_size, ball_size);
    //draw the player
    if (player_timeout == 0) {
      gb.display.setColor(GRAY);
    } else {
      gb.display.setColor(WHITE);
    }
    gb.display.fillRect(player_x, player_y, player_w, player_h);
    //draw the oponent
    if (oponent_timeout == 0) {
      gb.display.setColor(GRAY);
    } else {
      gb.display.setColor(WHITE);
    }
    gb.display.fillRect(oponent_x, oponent_y, oponent_w, oponent_h);
    //TRY ME! message
    if ((player_timeout == 0) && (oponent_timeout == 0)) {
      gb.display.setCursor(27, 16);
      gb.display.setColor(WHITE, BLACK);
      if ((gb.frameCount % 20) > 10) {
        gb.display.print("TRY ME!");
      }
    }
  }
}