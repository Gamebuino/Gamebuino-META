int highscore[NUM_HIGHSCORE];
char name[NUM_HIGHSCORE][NAMELENGTH+1];

const uint16_t highscore_sound[] PROGMEM = {
  0x0005,0x140,0x150,0x15C,0x170,0x180,0x16C,0x154,0x160,0x174,0x184,0x14C,0x15C,0x168,0x17C,0x18C,0x0000};

void initHighscore(){
  for(byte thisScore = 0; thisScore < NUM_HIGHSCORE; thisScore++){
    highscore[thisScore] = gb.save.get(2*thisScore);
    gb.save.get(2*thisScore + 1, name[thisScore], NAMELENGTH+1);
  }
}

void saveHighscore(unsigned int score){
  if(score < highscore[NUM_HIGHSCORE-1]){//if it's a highscore
    if(drawNewHighscore(score)){
      gb.getDefaultName(name[NUM_HIGHSCORE-1]);
      gb.keyboard(name[NUM_HIGHSCORE-1], NAMELENGTH+1);
      highscore[NUM_HIGHSCORE-1] = score;
      for(byte i=NUM_HIGHSCORE-1; i>0; i--){ //bubble sorting FTW
        if(highscore[i-1] > highscore[i]){
          char tempName[NAMELENGTH];
          strcpy(tempName, name[i-1]);
          strcpy(name[i-1], name[i]);
          strcpy(name[i], tempName);
          unsigned int tempScore;
          tempScore = highscore[i-1];
          highscore[i-1] = highscore[i];
          highscore[i] = tempScore;
        }
        else{
          break;
        }
      }
      for(byte thisScore = 0; thisScore < NUM_HIGHSCORE; thisScore++){
        gb.save.set(2*thisScore, highscore[thisScore]);
        gb.save.set(2*thisScore + 1, name[thisScore]);
      }
      drawHighScores();
    }
  }
  else{
    gb.popup("NEW LAP!",20);
  }
}

void drawHighScores(){
  while(true){
    if(gb.update()){
      gb.display.cursorX = 9+random(0,2);
      gb.display.cursorY = 0+random(0,2);
      gb.display.println("BEST TIMES");
      gb.display.textWrap = false;
      gb.display.cursorX = 0;
      gb.display.cursorY = gb.display.fontHeight;
      for(byte thisScore=0; thisScore<NUM_HIGHSCORE; thisScore++){
        if(highscore[thisScore]==0)
          gb.display.print('-');
        else
          gb.display.print(name[thisScore]);
        gb.display.cursorX = LCDWIDTH-4*gb.display.fontWidth;
        gb.display.cursorY = gb.display.fontHeight+gb.display.fontHeight*thisScore;
        gb.display.println(highscore[thisScore]);
      }
      if(gb.buttons.pressed(BTN_A) || gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C)){
        gb.sound.playOK();
        break;
      }
    }
  }
}

boolean drawNewHighscore(unsigned int score){
  //gb.sound.playPattern(highscore_sound, 0);
  gb.sound.playOK();
  while(1){
    if(gb.update()){
      gb.display.cursorX = 2+random(0,2);
      gb.display.cursorY = 0+random(0,2);
      gb.display.print("NEW HIGHSCORE");
      gb.display.cursorX = 0;
      gb.display.cursorY = 12;
      gb.display.print("Your time ");
      gb.display.print(score);
      gb.display.print("\nBest      ");
      gb.display.print(highscore[0]);
      gb.display.print("\nWorst     ");
      gb.display.print(highscore[NUM_HIGHSCORE-1]);
      gb.display.cursorX = 0;
      gb.display.cursorY = 40;
      gb.display.print("\25:Save \27:Exit");
      if(gb.buttons.pressed(BTN_A)){
        gb.sound.playOK();
        return true;
      }
      if(gb.buttons.pressed(BTN_C)){
        gb.sound.playCancel();
        return false;
      }
    }
  }
}
