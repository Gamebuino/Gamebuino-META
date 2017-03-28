#define PAUSEMENULENGTH 3
const char strPlay[] = "Play";
const char strHighScores[] = "High scores";
const char strMainMenu[] = "Main Menu";


const char* pauseMenu[PAUSEMENULENGTH] = {
  strPlay,
  strHighScores,
  strMainMenu
};

void drawMenu(){
  switch(gb.menu(pauseMenu, PAUSEMENULENGTH)){
  case 0: //play
    initGame();
    play();
    break;
  case 1: //high scores
    drawHighScores();
    break;
  default: //title screen
    drawTitleScreen();
    break;
  }
  
}
