typedef struct {
  float x, y, v, vx, vy, angle;
  byte radius;
} Ufo;

Ufo player;

void initPlayer(){
  player.radius = 3;
  player.x = 20;
  player.y = 20;
  player.v = 0;
  player.vx = 0;
  player.vy = 0;
  player.angle = 0;
}

void updatePlayer(){

  if(gb.buttons.repeat(BTN_RIGHT, 1)){
    player.angle += 0.31415/2;
  }
  if(gb.buttons.repeat(BTN_LEFT, 1)){
    player.angle -= 0.31415/2;
  }
  if(gb.buttons.repeat(BTN_A, 1)){
    player.v += 0.02;
  }
  if(gb.buttons.repeat(BTN_B, 1)){
    player.v *= 0.8;
    player.vx *= 0.8;
    player.vy *= 0.8;
  }

  byte currentTile = getTile(player.x/16, player.y/16);

  //friction
  switch(currentTile){
  case 0: //road
    player.v *= 0.95;
    player.vx *= 0.9;
    player.vy *= 0.9;
    break;
  case 5: //sand
    player.v *= 0.9;
    player.vx *= 0.8;
    player.vy *= 0.8;
    break;
  case 6: //ice
    player.v *= 0.7;
    player.vx *= 1;
    player.vy *= 1;
    break;
  default:
    player.v *= 0.95;
    player.vx *= 0.9;
    player.vy *= 0.9;
    break;
  }

  //accelerator
  switch(currentTile){
  case 1: //right
  case 7: //start (to avoid people to stop on it)
    player.vx += 1;
    break;
  case 2: //down
    player.vy += 1;
    break;
  case 3: //left
    player.vx -= 1;
    break;
  case 4: //up
    player.vy -= 1;
    break;
  }

  //speed update
  player.vx += cos(player.angle)*player.v;
  player.vy += sin(player.angle)*player.v;

  //collisions
  player.x += player.vx;
  currentTile = getTile(player.x/16, player.y/16);
  if(currentTile >= 8){
    player.x -= player.vx;
    if(currentTile == 8){//bouncer
      if(player.vx >= 0)
        player.vx = -6;
      else
        player.vx = 6;
      gb.sound.playOK();
    }
    else { //regular blocks
      player.vx *= -0.5;
      gb.sound.playTick();
    }
    player.v *= 0.5;
  }

  player.y += player.vy;
  currentTile = getTile(player.x/16, player.y/16);
  if(currentTile >= 8){
    player.y -= player.vy;
    if(currentTile == 8){//bouncer
      if(player.vy >= 0)
        player.vy = -6;
      else
        player.vy = 6;
      gb.sound.playOK();
    }
    else{ //regular blocks
      player.vy *= -0.5;
      gb.sound.playTick();
    }
    player.v *= 0.5;
  }

  //target position of the camera for the cursor to be centered
  int camera_x_target = player.x + cos(player.angle)*player.v*64 - LCDWIDTH/2;
  int camera_y_target = player.y + sin(player.angle)*player.v*64- LCDHEIGHT/2;
  //apply the target coordinate to the current coordinates with some smoothing
  camera_x = (camera_x*3 + camera_x_target)/4;
  camera_y = (camera_y*3 + camera_y_target)/4;

}

void drawPlayer(){
  int x_screen = (int)player.x - camera_x;
  int y_screen = (int)player.y - camera_y;
  if(!(x_screen < -16 || x_screen > LCDWIDTH || y_screen < -16 || y_screen > LCDHEIGHT)){
    gb.display.fillCircle(x_screen, y_screen, player.radius);
    gb.display.setColor(WHITE);
    gb.display.drawLine(x_screen,y_screen,x_screen+cos(player.angle)*4, y_screen+sin(player.angle)*4);
    gb.display.setColor(BLACK);
  }
}



