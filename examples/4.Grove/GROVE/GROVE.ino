#include <Gamebuino-Meta.h>

void setup(){
  gb.begin();
  //analog
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  //inputs
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  //outputs
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  
}

void loop(){
  gb.waitForUpdate();
  gb.display.clear();
  //gb.display.println(gb.frameCount);
  
  //INPUTS
  gb.display.setColor(GRAY);
  gb.display.print("- DIGITAL INPUTS -\n");
  gb.display.setColor(WHITE);
  gb.display.print("D0 D1 D2 D3 D4\n");
  if(digitalRead(0) == LOW){
    gb.display.setColor(RED);
    gb.display.print("LO ");
  } else {
    gb.display.setColor(BROWN);
    gb.display.print("HI ");
  }
  if(digitalRead(1) == LOW){
    gb.display.setColor(RED);
    gb.display.print("LO ");
  } else {
    gb.display.setColor(BROWN);
    gb.display.print("HI ");
  }
  if(digitalRead(2) == LOW){
    gb.display.setColor(RED);
    gb.display.print("LO ");
  } else {
    gb.display.setColor(BROWN);
    gb.display.print("HI ");
  }
  if(digitalRead(3) == LOW){
    gb.display.setColor(RED);
    gb.display.print("LO ");
  } else {
    gb.display.setColor(BROWN);
    gb.display.print("HI ");
  }
  if(digitalRead(4) == LOW){
    gb.display.setColor(RED);
    gb.display.print("LO ");
  } else {
    gb.display.setColor(BROWN);
    gb.display.print("HI ");
  }
  gb.display.print("\n");
  gb.display.cursorY += 3;

  //OUTPUTS
  gb.display.setColor(GRAY);
  gb.display.print("- DIGITAL OUTPUTS -\n");
  gb.display.setColor(WHITE);
  gb.display.print("D5 D6 D7 D8\n");
  if(gb.buttons.repeat(BUTTON_UP, 1)){
    digitalWrite(5, HIGH);
    gb.display.setColor(BROWN);
    gb.display.print("HI ");
  } else {
    digitalWrite(5, LOW);
    gb.display.setColor(RED);
    gb.display.print("LO ");
  }
  if(gb.buttons.repeat(BUTTON_RIGHT, 1)){
    digitalWrite(6, HIGH);
    gb.display.setColor(BROWN);
    gb.display.print("HI ");
  } else {
    digitalWrite(6, LOW);
    gb.display.setColor(RED);
    gb.display.print("LO ");
  }
  if(gb.buttons.repeat(BUTTON_DOWN, 1)){
    digitalWrite(7, HIGH);
    gb.display.setColor(BROWN);
    gb.display.print("HI ");
  } else {
    digitalWrite(7, LOW);
    gb.display.setColor(RED);
    gb.display.print("LO ");
  }
  if(gb.buttons.repeat(BUTTON_LEFT, 1)){
    digitalWrite(8, HIGH);
    gb.display.setColor(BROWN);
    gb.display.print("HI ");
  } else {
    digitalWrite(8, LOW);
    gb.display.setColor(RED);
    gb.display.print("LO ");
  }
  gb.display.print("\n");
  gb.display.cursorY += 3;

  //ANALOG INPUT
  gb.display.setColor(GRAY);
  gb.display.print("- ANALOG INPUT -\n");
  gb.display.setColor(WHITE);
  gb.display.print("A1   A2   A3   A4\n");
  gb.display.setColor(BROWN);
  gb.display.fillRect(4*5*0, gb.display.cursorY, map(analogRead(A1), 0,1023, 0,18), 3);
  gb.display.fillRect(4*5*1, gb.display.cursorY, map(analogRead(A2), 0,1023, 0,18), 3);
  gb.display.fillRect(4*5*2, gb.display.cursorY, map(analogRead(A3), 0,1023, 0,18), 3);
  gb.display.fillRect(4*5*3, gb.display.cursorY, map(analogRead(A4), 0,1023, 0,18), 3);
  gb.display.cursorY += 4;
  gb.display.setColor(GRAY);
  gb.display.print(analogRead(A1));
  gb.display.cursorX = 4*5*1;
  gb.display.print(analogRead(A2));
  gb.display.cursorX = 4*5*2;
  gb.display.print(analogRead(A3));
  gb.display.cursorX = 4*5*3;
  gb.display.print(analogRead(A4));
  gb.display.print("\n");
}