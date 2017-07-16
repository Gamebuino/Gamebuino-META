#define PAUSEMENULENGTH 2
const char strPlay[] = "START!";
const char strHighScores[] = "High scores";


const char* pauseMenu[PAUSEMENULENGTH] = {
  strPlay,
  strHighScores,
};

void drawMenu(){
  gb.display.setFont(font5x7);
  switch(gb.menu(pauseMenu, PAUSEMENULENGTH)){
  case 0: //play
    initGame();
    play();
    break;
  case 1: //high scores
    drawHighScores();
    break;
  } 
}
