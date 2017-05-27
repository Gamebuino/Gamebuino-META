/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "Gamebuino-Meta.h"
SdFat SD;

// a 3x5 font table
extern const uint8_t font3x5[];

namespace Gamebuino_Meta {


const uint16_t startupSound[] = {0x0005,0x3089,0x208,0x238,0x7849,0x1468,0x0000};


const uint8_t gamebuinoLogo[] =
{
	84,10, //width and height
	0b00000011, 0b11100001, 0b10000001, 0b10000110, 0b01111111, 0b00111110, 0b00011000, 0b01101101, 0b10000011, 0b00001111, 0b00001111, 
	0b00001110, 0b00000001, 0b10000011, 0b10000110, 0b01100000, 0b00110011, 0b00011000, 0b01101101, 0b11000011, 0b00011001, 0b10001111, 
	0b00011000, 0b00000011, 0b11000011, 0b10001110, 0b01100000, 0b00110011, 0b00011000, 0b01101101, 0b11100011, 0b00110000, 0b11001111, 
	0b00011000, 0b00000011, 0b11000011, 0b10011110, 0b01100000, 0b00110110, 0b00110000, 0b11001101, 0b11100011, 0b01100000, 0b11001111, 
	0b00110000, 0b00000110, 0b11000111, 0b10011110, 0b01111110, 0b00111110, 0b00110000, 0b11001101, 0b10110011, 0b01100000, 0b11001111, 
	0b00110000, 0b00001100, 0b11000110, 0b11110110, 0b01100000, 0b00110011, 0b00110000, 0b11011001, 0b10110110, 0b01100000, 0b11001111, 
	0b00110011, 0b11001111, 0b11001100, 0b11110110, 0b01100000, 0b01100001, 0b10110000, 0b11011011, 0b00011110, 0b01100000, 0b11001111, 
	0b00110000, 0b11011000, 0b01101100, 0b11100110, 0b11000000, 0b01100001, 0b10110000, 0b11011011, 0b00011110, 0b01100001, 0b10001111, 
	0b00011001, 0b10011000, 0b01101100, 0b11000110, 0b11000000, 0b01100011, 0b10110001, 0b10011011, 0b00001110, 0b00110011, 0b00001111, 
	0b00001111, 0b10110000, 0b01111000, 0b11000110, 0b11111111, 0b01111110, 0b00011111, 0b00011011, 0b00000110, 0b00011110, 0b00001111, 
};

void Gamebuino::begin() {
	// first we disable the watchdog timer so that we tell the bootloader everything is fine!
	WDT->CTRL.bit.ENABLE = 0;
	
	// let's to some sanity checks which are done on compile-time
	
	// check that the folder name length is at least 4 chars
#ifdef FOLDER_NAME
	static_assert(sizeof FOLDER_NAME - 1 >= 4, "your FOLDER_NAME must be at least 4 chars long!");
#else
	static_assert(sizeof __SKETCH_NAME__ - 1 >= 4 + 4, "Your sketch name is less than 4 chars long, please define FOLDER_NAME!");
	memcpy(folder_name, __SKETCH_NAME__, sizeof __SKETCH_NAME__ - 4);
	folder_name[sizeof __SKETCH_NAME__ - 4] = '\0';
#endif
	
	
	timePerFrame = 40;
	//nextFrameMillis = 0;
	//frameCount = 0;
	frameEndMicros = 1;
	startMenuTimer = 255;

	//battery
	battery = 3300;
	lowBattery = false;

	//neoPixels
	neoPixels.begin();
	neoPixels.clear();
	neoPixels.show();

	//buttons
	buttons.begin();
	buttons.update();
	
	//sound
	sound.begin();
	sound.playTick();
	
	//tft
	tft.initR(INITR_BLACKTAB);
	tft.setRotation(3);
	display.fillScreen(tft.Color565(127, 127, 127));

	tft.drawImage(0, 0, display, tft.width(), tft.height());
	tft.setColor(WHITE, BLACK);
	tft.print("SD INIT... ");
	if (!SD.begin(SD_CS)) {
		tft.setColor(RED, BLACK);
		tft.println("FAILED");
		delay(1000);
	} else {
		tft.setColor(GREEN, BLACK);
		tft.println("OK");
	}
	tft.setColor(WHITE, BLACK);
	
	// SD is initialized, let's switch to the folder!
	if (!SD.exists(folder_name)) {
		SD.mkdir(folder_name);
	}
	SD.chdir(folder_name);
}

void Gamebuino::titleScreen(const char* name){
	titleScreen(name, 0);
}

void Gamebuino::titleScreen(const uint8_t* logo){
	titleScreen("", logo);
}

void Gamebuino::titleScreen(){
	titleScreen("", 0);
}

void Gamebuino::titleScreen(const char*  name, const uint8_t *logo){
	display.fontSize = 1;
	display.textWrap = false;
	//display.persistence = false;
	//battery.show = false;
	display.setColor(BLACK);
	while(1){
		if(update()){
			uint8_t logoOffset = name[0]?display.fontHeight:0; //add an offset the logo when there is a name to display
			//draw graphics
			//#if LCDWIDTH == LCDWIDTH_NOROT
			display.drawBitmap(-1,1, gamebuinoLogo);
			if(logo){
				display.drawBitmap(0, 12+logoOffset, logo);
			}
			display.cursorX = 0;
			display.cursorY = 12;
			/*#else
			display.drawBitmap(7,0, gamebuinoLogo);
			display.drawBitmap(-41,12,gamebuinoLogo);
			if(logo){
				display.drawBitmap(0, 24+logoOffset, logo);
			}
			display.cursorX = 0;
			display.cursorY = 24;
			#endif*/
			display.cursorY = 12;
			
			display.print(name);
			
			//A button
			display.cursorX = LCDWIDTH - display.fontWidth*3 -1;
			display.cursorY = LCDHEIGHT - display.fontHeight*3 - 3;
			if((frameCount/16)%2)
			  display.println("\25 \20");
			else
			  display.println("\25\20 ");
			//B button
			display.cursorX = LCDWIDTH - display.fontWidth*3 - 1;
			display.cursorY++;
			if(sound.getVolume())
				display.println("\26\23\24");
			else
				display.println("\26\23x");
			//C button
			display.cursorX = LCDWIDTH - display.fontWidth*3 - 1;
			display.cursorY++;
			display.println("\27SD");
			
			//toggle volume when B is pressed
			if(buttons.pressed(Button::b)){
				sound.setVolume(sound.getVolume() + 1);
				sound.playTick();
			}
			//leave the menu
			if(buttons.pressed(Button::a)){
				//startMenuTimer = 255; //don't automatically skip the title screen next time it's displayed
				//sound.stopPattern(0);
				sound.playOK();
				break;
			}
			//flash the loader
			if(buttons.pressed(Button::c))
				changeGame();
		}
	}
	//battery.show = true;
}

bool recording_screen = false;

bool Gamebuino::update() {
	if (((nextFrameMillis - millis()) > timePerFrame) && frameEndMicros) { //if time to render a new frame is reached and the frame end has ran once
		nextFrameMillis = millis() + timePerFrame;
		frameCount++;

		frameEndMicros = 0;
		frameStartMicros = micros();

		buttons.update();

		return true;

	} else {
		if (!frameEndMicros) { //runs once at the end of the frame
		
			//draw and update popups
			updatePopup();

			//battery monitor
			analogReadResolution(10);
			//indirectly read VCC value, convert it to millivolts and smooth it out
			// - 1.2V: VDDCORE used as voltage reference, connected to A5 pin
			// - 1024: 10-bit analog resolution
			// - 1000: convert to millivolts to avoid the use of float
			// - low pass filter: weighted floating average
			//   averaging the previous value Y(N-1) with a weight of 7
			//   and the new value Y(N) with a weight of 1
			//   Y(N) = ( 7 * Y(N-1) + Y(N) ) / 8
			battery = ((7 * battery) + ((1024 * 1000 * 1.2) / analogRead(A5))) / 8;
			//if VCC drops under 3.1V, start showing the low battery indicator
			if (battery < 3100) lowBattery = true;
			//if VCC raises above 3.2V, stop showing the battery indicator
			if (battery > 3200) lowBattery = false;
			//actually draw the low battery indicator
			if (lowBattery) {
				display.setFont(font3x5);
				display.cursorX = display.width() - display.fontWidth + 1;
				display.cursorY = 0;
				display.setColor(BLACK);
				display.fillRect(display.cursorX - 1, 0, display.fontWidth, display.fontHeight);
				display.setColor(RED);
				if ((frameCount % 20) < 10) {
					//draw the empty battery character
					display.print("\7");
				}
				else {
					//draw the full battery character
					display.print("\11");
				}
				//disable light effects
				neoPixels.clear();
				//disable sound
				sound.setVolume(0);
			}
			
			//battery debug display
			/*
			display.setColor(GREEN, BLACK);
			if (battery < 3200) {
				tft.setColor(RED, BLACK);
			}
			//draw a  bar showing the voltage
			uint8_t barHeight = map(battery, 2500, 3300, 0, display.height() - display.fontHeight );
				display.drawFastVLine(display.width() - 1, display.height() - display.fontHeight  - barHeight, barHeight);
			display.cursorX = 0;
			display.cursorY = 6;
			display.print(battery);
			display.print("mV");
			*/

			//send buffer to the screen
			tft.drawImage(0, 0, display, tft.width(), tft.height()); //send the buffer to the screen

			//record screenshot
			if (buttons.pressed(Button::d)) {
				tft.setColor(RED,BLACK);
				tft.drawRect(0, 0, tft._width, tft._height);
				tft.drawRect(1, 1, tft._width - 2, tft._height - 2);
				if (recording_screen) {
					// stop the recording
					tft.setColor(WHITE, BLACK);
					tft.cursorX = 0;
					tft.cursorY = 2;
					
					tft.println("Processing screencapture...");
					sd_gfx.stopRecordImage(display, tft);
					recording_screen = false;
				} else {
					if (!sd_gfx.startRecordImage(display, "RECORD.BMP")) {
						tft.println("Couldn't start screen recording");
						while(1);
					}
					recording_screen = true;
				}
				
				/*
				tft.setColor(RED,BLACK);
				tft.drawRect(0, 0, tft._width, tft._height);
				tft.drawRect(1, 1, tft._width - 2, tft._height - 2);
				tft.cursorX = 0;
				tft.cursorY = 2;
				tft.setColor(WHITE, BLACK);
				sd_gfx.writeImage(display, "SCREEN.BMP");
				*/
			}

			sd_gfx.update(); // update screen recordings


			//if(!display.persistence)
			display.fillScreen(WHITE); //clear the buffer
			display.setCursor(0, 0);
			display.setColor(BLACK);

			//neoPixels update
			neoPixels.show();
			neoPixels.clear();

			frameEndMicros = micros(); //measure the frame's end time
			frameDurationMicros = frameEndMicros - frameStartMicros;

			//            display.setTextColor(BLACK);
			//            display.setCursor(0, 40);
			//            display.print(frameDurationMicros / timePerFrame);
			//            display.print(" ");
			//            display.print(2048 - freeRam());

			//            display.setCursor(0, 32);
			//            display.print("CPU:");
			//            display.print(frameDurationMicros / timePerFrame);
			//            display.println("/1000");
			//            display.print("RAM:");
			//            display.print(2048 - freeRam());
			//            display.println("/2048");
		}
		return false;
	}
}

void Gamebuino::setFrameRate(uint8_t fps) {
	timePerFrame = 1000 / fps;
}

void Gamebuino::pickRandomSeed(){
	//randomSeed(~battery.voltage * ~micros() * ~micros() + backlight.ambientLight + micros());
}

uint8_t Gamebuino::getCpuLoad(){
	return(frameDurationMicros/(10*timePerFrame));
}

extern "C" char* sbrk(int incr);	
uint16_t Gamebuino::getFreeRam() {	
	// from https://github.com/mpflaga/Arduino-MemoryFree/blob/master/MemoryFree.cpp
	char top;
	return &top - reinterpret_cast<char*>(sbrk(0));
}

int8_t Gamebuino::menu(const char* const* items, uint8_t length) {
#if (ENABLE_GUI > 0)
	//display.persistence = false;
	int8_t activeItem = 0;
	int8_t currentY = LCDHEIGHT;
	int8_t targetY = 0;
	bool exit = false;
	int8_t answer = -1;
	while (1) {
		if (update()) {
			if (buttons.pressed(Button::a) || buttons.pressed(Button::b) || buttons.pressed(Button::c)) {
				exit = true; //time to exit menu !
				targetY = - display.fontHeight * length - 2; //send the menu out of the screen
				if (buttons.pressed(Button::a)) {
					answer = activeItem;
					sound.playOK();
				} else {
					sound.playCancel();
				}
			}
			if (exit == false) {
				if (buttons.repeat(Button::down,4)) {
					activeItem++;
					sound.playTick();
				}
				if (buttons.repeat(Button::up,4)) {
					activeItem--;
					sound.playTick();
				}
				//don't go out of the menu
				if (activeItem == length) activeItem = 0;
				if (activeItem < 0) activeItem = length - 1;

				targetY = -display.fontHeight * activeItem + (display.fontHeight+4); //center the menu on the active item
			} else { //exit :
				if ((currentY - targetY) <= 1)
				return (answer);
			}
			//draw a fancy menu
			currentY = (currentY + targetY) / 2;
			display.cursorX = 0;
			display.cursorY = currentY;
			display.fontSize = 1;
			display.textWrap = false;
			for (byte i = 0; i < length; i++) {
				if (i == activeItem){
					display.cursorX = 3;
					display.cursorY = currentY + display.fontHeight * activeItem;
				}
				display.println((const char*)items[i]);
			}

			//display.fillRect(0, currentY + 3 + 8 * activeItem, 2, 2, BLACK);
			display.setColor(WHITE);
			display.drawFastHLine(0, currentY + display.fontHeight * activeItem - 1, LCDWIDTH);
			display.setColor(BLACK);
			display.drawRoundRect(0, currentY + display.fontHeight * activeItem - 2, LCDWIDTH, (display.fontHeight+3), 3);
		}
	}
#else
	return 0;
#endif
}

void Gamebuino::keyboard(char* text, uint8_t length) {
#if (ENABLE_GUI > 0)
	//display.persistence = false;
	//memset(text, 0, length); //clear the text
	text[length-1] = '\0';
	//active character in the typing area
	int8_t activeChar = 0;
	//selected char on the keyboard
	int8_t activeX = 0;
	int8_t activeY = 2;
	//position of the keyboard on the screen
	int8_t currentX = LCDWIDTH;
	int8_t currentY = LCDHEIGHT;
	int8_t targetX = 0;
	int8_t targetY = 0;

	while (1) {
		if (update()) {
			//move the character selector
			if (buttons.repeat(Button::down, 4)) {
				activeY++;
				sound.playTick();
			}
			if (buttons.repeat(Button::up, 4)) {
				activeY--;
				sound.playTick();
			}
			if (buttons.repeat(Button::right, 4)) {
				activeX++;
				sound.playTick();
			}
			if (buttons.repeat(Button::left, 4)) {
				activeX--;
				sound.playTick();
			}
			//don't go out of the keyboard
			if (activeX == KEYBOARD_W) activeX = 0;
			if (activeX < 0) activeX = KEYBOARD_W - 1;
			if (activeY == KEYBOARD_H) activeY = 0;
			if (activeY < 0) activeY = KEYBOARD_H - 1;
			//set the keyboard position on screen
			targetX = -(display.fontWidth+1) * activeX + LCDWIDTH / 2 - 3;
			targetY = -(display.fontHeight+1) * activeY + LCDHEIGHT / 2 - 4 - display.fontHeight;
			//smooth the keyboard displacement
			currentX = (targetX + currentX) / 2;
			currentY = (targetY + currentY) / 2;
			//type character
			if (buttons.pressed(Button::a)) {
				if (activeChar < (length-1)) {
					byte thisChar = activeX + KEYBOARD_W * activeY;
					if((thisChar == 0)||(thisChar == 10)||(thisChar == 13)) //avoid line feed and carriage return
					continue;
					text[activeChar] = thisChar;
					text[activeChar+1] = '\0';
				}
				activeChar++;
				sound.playOK();
				if (activeChar > length)
				activeChar = length;
			}
			//erase character
			if (buttons.pressed(Button::b)) {
				activeChar--;
				sound.playCancel();
				if (activeChar >= 0)
				text[activeChar] = 0;
				else
				activeChar = 0;
			}
			//leave menu
			if (buttons.pressed(Button::c)) {
				sound.playOK();
				while (1) {
					if (update()) {
						//display.setCursor(0,0);
						display.println("You entered\n");
						display.print(text);
						display.println("\n\n\n\x15:okay \x16:edit");
						if(buttons.pressed(Button::a)){
							sound.playOK();
							return;
						}
						if(buttons.pressed(Button::b)){
							sound.playCancel();
							break;
						}
					}
				}
			}
			//draw the keyboard
			for (int8_t y = 0; y < KEYBOARD_H; y++) {
				for (int8_t x = 0; x < KEYBOARD_W; x++) {
					display.drawChar(currentX + x * (display.fontWidth+1), currentY + y * (display.fontHeight+1), x + y * KEYBOARD_W, 1);
				}
			}
			//draw instruction
			display.cursorX = currentX-display.fontWidth*6-2;
			display.cursorY = currentY+1*(display.fontHeight+1);
			display.print("\25type");
			
			display.cursorX = currentX-display.fontWidth*6-2;
			display.cursorY = currentY+2*(display.fontHeight+1);
			display.print("\26back");
			
			display.cursorX = currentX-display.fontWidth*6-2;
			display.cursorY = currentY+3*(display.fontHeight+1);
			display.print("\27save");
			
			//erase some pixels around the selected character
			display.setColor(WHITE);
			display.drawFastHLine(currentX + activeX * (display.fontWidth+1) - 1, currentY + activeY * (display.fontHeight+1) - 2, 7);
			//draw the selection rectangle
			display.setColor(BLACK);
			display.drawRoundRect(currentX + activeX * (display.fontWidth+1) - 2, currentY + activeY * (display.fontHeight+1) - 3, (display.fontWidth+2)+(display.fontWidth-1)%2, (display.fontHeight+5), 3);
			//draw keyboard outline
			//display.drawRoundRect(currentX - 6, currentY - 6, KEYBOARD_W * (display.fontWidth+1) + 12, KEYBOARD_H * (display.fontHeight+1) + 12, 8, BLACK);
			//text field
			display.drawFastHLine(0, LCDHEIGHT-display.fontHeight-2, LCDWIDTH);
			display.setColor(WHITE);
			display.fillRect(0, LCDHEIGHT-display.fontHeight-1, LCDWIDTH, display.fontHeight+1);
			//typed text
			display.cursorX = 0;
			display.cursorY = LCDHEIGHT-display.fontHeight;
			display.setColor(BLACK);
			display.print(text);
			//blinking cursor
			if (((frameCount % 8) < 4) && (activeChar < (length-1)))
			display.drawChar(display.fontWidth * activeChar, LCDHEIGHT-display.fontHeight, '_',1);
		}
	}
#endif
}

void Gamebuino::popup(const char* text, uint8_t duration){
#if (ENABLE_GUI > 0)
	popupText = text;
	popupTimeLeft = duration+12;
#endif
}

void Gamebuino::updatePopup(){
#if (ENABLE_GUI > 0)
	if (popupTimeLeft){
		uint8_t yOffset = 0;
		if(popupTimeLeft<12){
			yOffset = 12-popupTimeLeft;
		}
		display.fontSize = 1;
		display.setColor(WHITE);
		display.fillRoundRect(0,LCDHEIGHT-display.fontHeight+yOffset-3,LCDWIDTH,display.fontHeight+3,3);
		display.setColor(BLACK);
		display.drawRoundRect(0,LCDHEIGHT-display.fontHeight+yOffset-3,LCDWIDTH,display.fontHeight+3,3);
		display.cursorX = 4;
		display.cursorY = LCDHEIGHT-display.fontHeight+yOffset-1;
		display.print(popupText);
		popupTimeLeft--;
	}
#endif
}

void Gamebuino::changeGame(){
	//display a "loading" message
	// unneeded as the bootloader does that when flashing loader.bin
	/*
	display.fontSize = 1;
	display.cursorX = 0;
	display.cursorY = 0;
	display.fillScreen(BLACK);
	display.setColor(WHITE);
	display.print("LOADING...");
	tft.drawImage(0, 0, display, tft.width(), tft.height());
	*/
	//flash loader.bin
	load_loader();
}

void Gamebuino::getDefaultName(char* string){
	return;
}

bool Gamebuino::collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h){
	if((x1>=x2)&&(x1<x2+w))
	if((y1>=y2)&&(y1<y2+h))
	return true;
	return false;
}

bool Gamebuino::collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2){
  return !( x2     >=  x1+w1  || 
            x2+w2  <=  x1     || 
            y2     >=  y1+h1  ||
            y2+h2  <=  y1     );
}

bool Gamebuino::collideBitmapBitmap(int16_t x1, int16_t y1, const uint8_t* b1, int16_t x2, int16_t y2, const uint8_t* b2){
  int16_t w1 = b1[0];
  int16_t h1 = b1[1];
  int16_t w2 = b2[0];
  int16_t h2 = b2[1];

  if(collideRectRect(x1, y1, w1, h1, x2, y2, w2, h2) == false){
    return false;
  }
  
  int16_t xmin = (x1>=x2)? 0 : x2-x1;
  int16_t ymin = (y1>=y2)? 0 : y2-y1;
  int16_t xmax = (x1+w1>=x2+w2)? x2+w2-x1 : w1;
  int16_t ymax = (y1+h1>=y2+h2)? y2+h2-y1 : h1;

  for(uint8_t y = ymin; y < ymax; y++){
    for(uint8_t x = xmin; x < xmax; x++){
      if(display.getBitmapPixel(b1, x, y) && display.getBitmapPixel(b2, x1+x-x2, y1+y-y2)){
        return true;
      }
    }
  }

  return false;
}


} // namespace Gamebuino_Meta

Gamebuino gb;
