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

#include "Buttons.h"

namespace Gamebuino_Meta {

void Buttons::begin() {
  SPI.begin();
  pinMode(BTN_CS, OUTPUT);
  digitalWrite(BTN_CS, HIGH);
}

/*
 * reads each button states and store it
 */
void Buttons::update() {
	
	//start SPI
	SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));
	digitalWrite(BTN_CS, LOW);
	//wait for PL to recover
	delayMicroseconds(1);
	//get the buttons states from the shift register
	byte buttonsData = SPI.transfer(1);
	//end SPI
	digitalWrite(BTN_CS, HIGH);
	SPI.endTransaction();
	//Print raw data to native USB
	//SerialUSB.println(buttonsData,BIN);
  
    for (uint8_t thisButton = 0; thisButton < NUM_BTN; thisButton++) {
		//extract the corresponding bit corresponding to the current button
		//Inverted logic : button pressed = low state = 0
		boolean pressed = (buttonsData & (1 << thisButton)) == 0;
		
        if (pressed) { //if button pressed
            states[thisButton]++; //increase button hold time
        } else {
            if (states[thisButton] == 0)//button idle
                continue;
            if (states[thisButton] == 0xFF)//if previously released
                states[thisButton] = 0; //set to idle
            else
                states[thisButton] = 0xFF; //button just released
        }
    }
}

/*
 * Returns true when 'button' is pressed.
 * The button has to be released for it to be triggered again.
 */
boolean Buttons::pressed(uint8_t button) {
    if (states[button] == 1)
        return true;
    else
        return false;
}

/*
 * return true if 'button' is released
 */
boolean Buttons::released(uint8_t button) {
    if (states[button] == 0xFF)
        return true;
    else
        return false;
}

/**
 * returns true ONCE when 'button' is held for 'time' frames
 * @param button The button's ID
 * @param time How much frames button must be held, between 1 and 254.
 * @return true when 'button' is held for 'time' frames
 */
boolean Buttons::held(uint8_t button, uint8_t time){
    if(states[button] == (time+1))
        return true;
    else
        return false;
}

/**
 * returns true every 'period' frames when 'button' is held
 * @param button The button's ID
 * @param period How much frames button must be held, between 1 and 254.
 * @return true if the button is held for the given time
 */
boolean Buttons::repeat(uint8_t button, uint8_t period) {
    if (period <= 1) {
        if ((states[button] != 0xFF) && (states[button]))
            return true;
    } else {
        if ((states[button] != 0xFF) && ((states[button] % period) == 1))
            return true;
    }
    return false;
}

/**
 * 
 * @param button The button's ID
 * @return The number of frames during which the button has been held.
 */
uint8_t Buttons::timeHeld(uint8_t button){
    if(states[button] != 0xFF)
        return states[button];
    else
        return 0;
    
}

} // Gamebuino_Meta
