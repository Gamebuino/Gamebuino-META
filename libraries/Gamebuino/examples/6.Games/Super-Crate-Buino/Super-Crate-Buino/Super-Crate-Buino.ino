/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * You can redistribute this program and/or modify
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


//#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

//prototypes
void popup(const __FlashStringHelper* text, uint8_t duration);
void popup(const __FlashStringHelper* text);
void updatePopup();

//Warning : this game require 3 channels, please set #NUM_CHANNELS 3 in libraries/gamebuino/settings.c or you won't hear all the sounds

const byte logo[] PROGMEM = {64,30,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xFF,0xFF,0xFF,0xFF,0xF0,0x0,0x0,0x0,0x81,0x32,0x4,0x8,0x10,0x0,0x0,0x0,0x9F,0x32,0x64,0xF9,0x90,0x0,0x0,0x0,0x81,0x32,0x4,0x38,0x30,0x0,0x0,0x0,0xF9,0x32,0x7C,0xF9,0x90,0x0,0x0,0x0,0x81,0x2,0x7C,0x9,0x90,0x0,0x0,0x0,0xFF,0xFF,0xFF,0xFF,0xF0,0x0,0x0,0x0,0x81,0x2,0x4,0x8,0x10,0x0,0x0,0x0,0x9F,0x32,0x67,0x39,0xF0,0x0,0x0,0x0,0x9F,0x6,0x7,0x38,0x70,0x0,0x0,0x0,0x9F,0x32,0x67,0x39,0xF0,0x0,0x0,0x0,0x81,0x32,0x67,0x38,0x10,0x0,0x0,0x0,0xFF,0xFF,0xFF,0xFF,0xF0,0x0,0x0,0x0,0x60,0x4C,0x93,0x20,0x60,0x60,0x0,0x0,0x26,0x4C,0x91,0x26,0x40,0x1,0xF8,0x0,0x20,0xCC,0x92,0x26,0x40,0xD,0xF8,0x20,0xA6,0x4C,0x93,0x26,0x40,0x81,0xF,0xE4,0x20,0x40,0x93,0x20,0x40,0x1,0xBF,0x80,0x3F,0xFF,0xFF,0xFF,0xC0,0x1,0xF8,0x0,0x7,0xFF,0xFF,0xFE,0x0,0x1,0xF8,0x0,0x0,0xFF,0xFF,0xF0,0x0,0x1,0xF8,0x0,0x0,0x1F,0xFF,0x80,0x0,0x1,0x98,0x0,0x0,0x3,0xFC,0x0,0x0,0xFF,0xFF,0xF0,0x0,0x0,0x60,0x0,0x0,0x0,0x0,0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,0x0,0x0,0x0,0x0,0x0,0xC3,0xC,0x8,0x0,0x0,0x0,0x0,0x0,0x34,0xD3,0x48,0x0,0x0,0x0,0x0,0x0,0xCB,0x2C,0xB8,0x0,0x0,0x0,0x0,0x0,};

//EEPROM offsets
#define EEPROM_TOKEN 0xCAB2
#define EEPROM_WEAPONS_OFFSET 0x004
#define EEPROM_MAPS_OFFSET 0x006
#define EEPROM_SCORE_OFFSET 0x0020

//Weapons and bullets subtypes
#define NUMWEAPONS 13
#define W_PISTOL 0
#define W_RIFLE 1
#define W_SHOTGUN 2
#define W_ROCKET 3
#define W_CLUB 4
#define W_REVOLVER 5
#define W_MINE 6
#define W_SNIPER 7
#define W_MACHINEGUN 8
#define W_GRENADE 9
#define W_AKIMBO 10
#define W_DISK 11
#define W_LASER 12
//can't be held by the player as a weapon :
#define W_EXPLOSION 13
#define W_SHELL 14

byte unlockedWeapons = 0;
byte unlockedMaps = 0;

#define SCORETHRESHOLD_1 4
#define SCORETHRESHOLD_2 8
#define SCORETHRESHOLD_3 12
#define SCORETHRESHOLD_4 14
#define SCORETHRESHOLD_5 16
const byte scoreThresholds[] = {SCORETHRESHOLD_1, SCORETHRESHOLD_2, SCORETHRESHOLD_3, SCORETHRESHOLD_4, SCORETHRESHOLD_5};

//Enemy types
#define E_SMALL 0
#define E_BIG 1

//there is a scale between the world cccordinates and the screen coordinates
//to allow sub pixel accuracy without the use of floats
#define SCALE 8

int cameraX, cameraY, shakeTimeLeft, shakeAmplitude;
byte popupTimeLeft;
const char* popupText;

int toScreenX(int x) {
  return x / SCALE - cameraX;
}

int toScreenY(int y) {
  return y / SCALE - cameraY;
}

///////////////////////////////////////////// SOUNDS

const uint16_t player_damage_sound[] PROGMEM = {0x0045, 0x564, 0x0000};
const uint16_t revolver_sound[] PROGMEM = {0x0045, 0x7049, 0x17C, 0x784D, 0x42C, 0x0000};
const uint16_t grenade_sound[] PROGMEM = {0x0045, 0x012C, 0x0000};
const uint16_t machinegun_sound[] PROGMEM = {0x0045, 0x140, 0x8141, 0x7849, 0x788D, 0x52C, 0x0000};
const uint16_t rocket_sound[] PROGMEM = {0x8045, 0x8001, 0x8889, 0x3C5C, 0x0000};
const uint16_t blast_sound[] PROGMEM = {0x0045, 0x7849, 0x784D, 0xA28, 0x0000};
const uint16_t power_up_sound[] PROGMEM = {0x0005, 0x140, 0x150, 0x15C, 0x170, 0x180, 0x16C, 0x154, 0x160, 0x174, 0x184, 0x14C, 0x15C, 0x168, 0x17C, 0x18C, 0x0000};
const uint16_t enemy_death_sound[] PROGMEM = {0x0045, 0x184, 0x0000};
const uint16_t jump_sound[] PROGMEM = {0x0005, 0x7049, 0x884D, 0x354, 0x0000};
const uint16_t enemy_felt_sound[] PROGMEM = {0x8005, 0x8001, 0x8849, 0xF20, 0x0000};
const uint16_t shotgun_sound[] PROGMEM = {0x0045, 0x7049, 0x334, 0x0000};
const uint16_t laser_sound[] PROGMEM = {0x0005, 0x784D, 0x7849, 0x670, 0x0000};
const unsigned int club_sound[] PROGMEM = {0x8005, 0x784D, 0x7849, 0x318, 0x0000};





///////////////////////////////////////////// WORLD
#define SPRITE_SIZE 6

#define BG_BLOCK 1
#define BG_FENCE 2
#define BG_TREE 3
#define BG_BLOCKTOP 4
#define BG_BLOCKCORNER 5
#define BG_TREEMIDDLE 6
#define BG_TREEBOTTOM 7

//maps are encoded like bitmaps
const byte map0[] PROGMEM = {
  16, 11,
  B11111110, B01111111,
  B10000000, B00000001,
  B10000000, B00000001,
  B10001111, B11110001,
  B10000000, B00000001,
  B10000000, B00000001,
  B11111100, B00111111,
  B10000000, B00000001,
  B10000000, B00000001,
  B10011111, B11111001,
  B10011111, B11111001,
};

const byte map1[] PROGMEM = {
  24, 16,
  B11111111, B11100111, B11111111,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B10000011, B11111111, B11000001,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B11111100, B00000000, B00111111,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B10000111, B11111111, B11100001,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B11111000, B00000000, B00011111,
  B11111111, B11000011, B11111111,
};

const byte map2[] PROGMEM = {
  24, 16,
  B11111111, B11100111, B11111111,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B10000000, B00111100, B00000001,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B10000111, B11100111, B11100001,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B11111100, B00011000, B00111111,
  B11100000, B00000000, B00000111,
  B11100000, B00000000, B00000111,
  B11100000, B11111111, B00000111,
  B11100000, B00000000, B00000111,
  B11100000, B00000000, B00000111,
  B11111111, B11000011, B11111111,
};

const byte map3[] PROGMEM = {
  24, 16,
  B11111111, B11100111, B11111111,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B10000011, B11111111, B11000001,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B11111000, B00000000, B00011111,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B10001111, B11000011, B11110001,
  B10000000, B00000000, B00000001,
  B10000000, B00000000, B00000001,
  B11110000, B00111100, B00001111,
  B11110011, B11111111, B11001111,
  B11110011, B11111111, B11001111,
};

const byte map4[] PROGMEM = {
  32, 24,
  B11111111, B11111110, B01111111, B11111111,
  B10000000, B00000000, B00000000, B00000001,
  B10000000, B00000000, B00000000, B00000001,
  B11110000, B00000111, B11100000, B00001111,
  B10000000, B00000000, B00000000, B00000001,
  B10000000, B00000000, B00000000, B00000001,
  B10000111, B11111100, B00111111, B11100001,
  B10000000, B00000000, B00000000, B00000001,
  B10000000, B00000000, B00000000, B00000001,
  B11110000, B00000011, B11000000, B00000001,
  B10000000, B00000011, B11000001, B11111111,
  B10000000, B00000011, B11000000, B00111111,
  B10001111, B11111111, B11000000, B00000011,
  B10000000, B00000011, B11111100, B00000011,
  B10000000, B00000000, B00000000, B00000011,
  B11111100, B00000000, B00000000, B00000011,
  B11000000, B00001100, B00000001, B11111111,
  B11000000, B00001100, B00000000, B00111111,
  B11001111, B11111100, B00000011, B00001111,
  B11000000, B00001111, B00000011, B00001111,
  B11000000, B00000011, B11000000, B00001111,
  B11111100, B00000000, B00000000, B00001111,
  B11111111, B11000000, B00000000, B00001111,
  B11111111, B11111111, B11111111, B00001111,
};

#define NUMMAPS 5
const byte* const maps[NUMMAPS] = {
  map0, map1, map2, map3, map4
};

unsigned int score[NUMMAPS];

const byte bricks[] PROGMEM = {
  8, 6, 0xFC, 0x24, 0xFC, 0x90, 0xFC, 0x48
};
const byte grass[] PROGMEM = {
  8, 6, 0xFC, 0x0, 0x0, 0x18, 0xA4, 0x58
};
const byte grass_edge[] PROGMEM = {
  8, 6, 0x7C, 0x80, 0x80, 0x80, 0x94, 0xE8
};
const byte beam[] PROGMEM = {
  8, 6, 0xFC, 0x10, 0x28, 0x44, 0x80, 0xFC
};
const byte roundPlatform[] PROGMEM = {
  8, 6, 0xFC, 0x0, 0x0, 0x0, 0xFC, 0xFC
};
const byte roundPlatform_edge[] PROGMEM = {
  8, 6, 0x7C, 0xE0, 0xC0, 0xE0, 0xFC, 0x7C,
};
const byte largeChecker[] PROGMEM = {
  8, 6, 0x1C, 0x1C, 0x1C, 0xE0, 0xE0, 0xE0,
};
const byte blackWall[] PROGMEM = {
  8, 6, 0xF4, 0xFC, 0xBC, 0xF8, 0xFC, 0xDC,
};


class World {
  public:
    const byte* tiles;
    const byte* wall; //pointer to the used tilemap
    const byte* platform;
    const byte* edge;
    byte mapNumber;

    int getWidth() {
      return SPRITE_SIZE * pgm_read_byte(tiles) * SCALE;
    }
    int getHeight() {
      return SPRITE_SIZE * pgm_read_byte(tiles + 1) * SCALE;
    }

    byte tileAtPosition(int x, int y) {
      int tile_x = (x - SCALE / 2) / SPRITE_SIZE / SCALE;
      int tile_y = (y - SCALE / 2) / SPRITE_SIZE / SCALE;

      int w = pgm_read_byte(tiles);
      int h = pgm_read_byte(tiles + 1);

      // Offmap is made of air
      if (tile_x < 0 || tile_x >= w || tile_y < 0 || tile_y >= h)
        return 0;

      //maps are encoded like bitmaps so we can use getBitmapPixel
      return gb.display.getBitmapPixel(tiles, tile_x, tile_y) > 0 ? 1 : 0;
    }

    boolean solidCollisionAtPosition(int16_t x, int16_t y, int16_t w, int16_t h) {
      if (tileAtPosition(x, y + h))
        return true;
      if (tileAtPosition(x + w, y + h))
        return true;
      if (tileAtPosition(x + w, y))
        return true;
      if (tileAtPosition(x, y))
        return true;
      //also check at half the length for boxes larger than a sprite
      if (w > SPRITE_SIZE * SCALE) {
        if (tileAtPosition(x + w / 2, y))
          return true;
        if (tileAtPosition(x + w / 2, y + h))
          return true;
      }
      if (h > SPRITE_SIZE * SCALE) {
        if (tileAtPosition(x, y + h / 2))
          return true;
        if (tileAtPosition(x + w, y + h / 2))
          return true;
      }
      //check all the points
      /*if((w > SPRITE_SIZE*SCALE) || (h > SPRITE_SIZE*SCALE)){
       for (int i = x; i <= (x + w); i += SPRITE_SIZE*SCALE){
       for (int j = y; j <= (y + h); j += SPRITE_SIZE*SCALE){
       if (tileAtPosition(i, j))
       return true;
       }
       }
       }*/
      return false;
    }

    void draw() {
      int xMin = cameraX / SPRITE_SIZE;
      int xMax = (LCDWIDTH / SPRITE_SIZE) + (cameraX / SPRITE_SIZE) + 2;
      int yMin = cameraY / SPRITE_SIZE;
      int yMax = (LCDHEIGHT / SPRITE_SIZE) + (cameraY / SPRITE_SIZE) + 2;

      int w = pgm_read_byte(tiles);
      int h = pgm_read_byte(tiles + 1);

      for (int y = yMin; y < yMax; y++) {
        for (int x = xMin; x < xMax; x++ ) {
          if (x < 0 || x >= w || y < 0 || y >= h)
            continue;
          if (gb.display.getBitmapPixel(tiles, x, y)) {
            byte tileNumber = 1; //platform by default
            byte flip = NOFLIP;
            byte offset = 0;
            const byte* bitmap = platform;
            if (y >= getHeight() / SPRITE_SIZE / SCALE - 1 || y <= 0 || gb.display.getBitmapPixel(tiles, x, y - 1)) {
              bitmap = wall; //bricks
            }
            else {
              if (edge) {
                if (y > 0 && !gb.display.getBitmapPixel(tiles, x + 1, y)) {
                  bitmap = edge; //platform corner
                  flip = FLIPH;
                  offset = 2;
                }
                if (y > 0 && !gb.display.getBitmapPixel(tiles, x - 1, y)) {
                  bitmap = edge; //platform corner
                }
              }
            }
            gb.display.drawBitmap(x * SPRITE_SIZE - cameraX - offset, y * SPRITE_SIZE - cameraY, bitmap, NOROT, flip);
          }
        }
      }
    }

    void chooseMap() {
      int thisMap = mapNumber;
      while (1) {
        if (gb.update()) {

          //assign the selected map
          tiles = maps[thisMap];
          mapNumber = thisMap;

          switch (thisMap) {
            case 1:
              wall = bricks;
              platform = beam;
              edge = 0;
              break;
            case 0:
            case 2:
            case 4:
              wall = bricks;
              platform = grass;
              edge = grass_edge;
              break;
            case 3:
              wall = blackWall;
              platform = roundPlatform;
              edge = roundPlatform_edge;
              break;

          }

          gb.display.cursorY = LCDHEIGHT - 17;
          printCentered("\21 Select map \20");
          gb.display.cursorX = 24;
          gb.display.cursorY = LCDHEIGHT - 11;
          gb.display.print("Score: ");
          gb.display.print(score[thisMap]);
          //draw the map centered on the screen
          gb.display.drawBitmap(LCDWIDTH / 2 - getWidth() / 2 / SCALE / SPRITE_SIZE, LCDHEIGHT / 2 - getHeight() / 2 / SCALE / SPRITE_SIZE - 5, maps[thisMap]);

          for (byte x = SPRITE_SIZE; x < LCDWIDTH - SPRITE_SIZE; x += SPRITE_SIZE) {
            gb.display.drawBitmap(x, 0, platform);
          }
          for (byte y = SPRITE_SIZE; y < LCDHEIGHT; y += SPRITE_SIZE) {
            gb.display.drawBitmap(0, y, wall);
            gb.display.drawBitmap(LCDWIDTH - SPRITE_SIZE, y, wall);
          }
          if (edge) { //draw the ends with the according bitmap
            gb.display.drawBitmap(0, 0, edge);
            gb.display.drawBitmap(LCDWIDTH - SPRITE_SIZE - 2, 0, edge, NOROT, FLIPH);
          }
          else { //draw with end with a regular platform if there is no edge bitmap
            gb.display.drawBitmap(0, 0, platform);
            gb.display.drawBitmap(LCDWIDTH - SPRITE_SIZE, 0, platform);
          }
          if (thisMap == unlockedMaps) {
            for (byte i = 0; i < sizeof(scoreThresholds); i++) {
              if (score[thisMap] < scoreThresholds[i]) {
                gb.display.cursorY = LCDHEIGHT - 5;
                gb.display.cursorX = 12;
                if ((gb.frameCount % 10) > 3) { //make it blink !
                  gb.display.print("Next unlock: ");
                  gb.display.print(scoreThresholds[i]);
                }
                break;
              }
            }
          }
          if (thisMap > unlockedMaps) {
            if ((gb.frameCount % 10) > 3) { //make it blink !
              gb.display.setColor(BLACK);
              gb.display.fillRect(28, 15, 28, 7);
              gb.display.setColor(WHITE);
              gb.display.cursorX = 29;
              gb.display.cursorY = 16;
              gb.display.print("LOCKED!");
              gb.display.setColor(BLACK);
            }
          }
          if (gb.buttons.pressed(BTN_A) && (thisMap <= unlockedMaps)) {
            initGame();
            return;
          }
          if (gb.buttons.pressed(BTN_RIGHT))
            thisMap = (thisMap + 1) % NUMMAPS;
          if (gb.buttons.pressed(BTN_LEFT))
            thisMap = (thisMap - 1 + NUMMAPS) % NUMMAPS;
          if (gb.buttons.pressed(BTN_C) || gb.buttons.pressed(BTN_B)) {
            mainMenu();
          }
        }
      }
    }

    boolean addScore(unsigned int newScore) {
      if (newScore > score[mapNumber]) {
        score[mapNumber] = newScore;
        return true;
      }
      return false;
    }
};

World world;

/////////////////////////////////////////////
///////////////////////////////////////////// BOX
/////////////////////////////////////////////

class Box {
  public:
    int x, y, vx, vy;
    int dir;
    virtual byte getType() {
      return 0;
    };
    virtual int getWidth() {
      return 128;
    };
    virtual int getHeight() {
      return 128;
    };
    virtual int getGravity() {
      return 8;
    };
    virtual int getMaxSpeed() {
      return 128;
    }
    virtual int getXFriction() {
      return 5;
    }
    virtual int getYFriction() {
      return 5;
    }
    virtual int getXBounce() {
      return 100;
    }
    virtual int getYBounce() {
      return 100;
    }

    byte update() {
      vy += getGravity();
      vx = (vx * (100 - getXFriction())) / 100;
      vy = (vy * (100 - getYFriction())) / 100;
      vx = constrain(vx, - getMaxSpeed(), getMaxSpeed());
      vy = constrain(vy, - getMaxSpeed(), getMaxSpeed());
      boolean collided = 0;

      x += vx;
      if (getXBounce() >= 0) {
        int vxdir = vx > 0 ? 1 : -1;
        if (world.solidCollisionAtPosition(x, y, getWidth(), getHeight())) {
          collided = 1;
          do {
            x -= vxdir; //move back step by step until we get out of the colision
          }
          while (world.solidCollisionAtPosition(x, y, getWidth(), getHeight()));
          vx = -(vx * getXBounce()) / 100;
        }
      }

      y += vy;
      if (getXBounce() >= 0) {
        int vydir = vy > 0 ? 1 : -1;
        if (world.solidCollisionAtPosition(x, y, getWidth(), getHeight())) {
          collided = 1;
          do {
            y -= vydir;
          }
          while (world.solidCollisionAtPosition(x, y, getWidth(), getHeight()));
          vy = -(vy * getYBounce()) / 100;
        }
      }

      if (vx > 0) {
        dir = 1;
      }
      if (vx < 0) {
        dir = -1;
      }

      return collided;
    }

    boolean isOffScreen() {
      return (toScreenX(x) + toScreenX(x + getWidth()) < 0) || (toScreenX(x) > LCDWIDTH) || (toScreenY(y) + toScreenY(y + getHeight()) < 0) || (toScreenY(y) > LCDHEIGHT);
    }

    void draw() {
      if (isOffScreen())
        return; //skip boxes which are out of the screen
      gb.display.fillRect(toScreenX(x), toScreenY(y), getWidth() / SCALE, getHeight() / SCALE);
    }
};

/////////////////////////////////////////////
///////////////////////////////////////////// BULLETS
/////////////////////////////////////////////

class Bullet :
  public Box {
  public:
    byte subtype;
    int timeLeft;

    int getWidth() {
      switch (subtype) {
        case W_CLUB :
          return 96;
        case W_PISTOL :
        case W_AKIMBO :
        case W_RIFLE :
        case W_SHOTGUN :
          return constrain(abs(vx), 8, 16);
        case W_MACHINEGUN :
          return constrain(abs(vx), 8, 24);
        case W_REVOLVER :
        case W_SNIPER :
          return constrain(abs(vx), 8, 32);
        case W_DISK :
          return min(timeLeft * 8, 48);
        case W_LASER :
          return 96;
        case W_GRENADE :
          return 24;
        case W_ROCKET :
          return 48;
        case W_MINE :
          return 32;
        case W_EXPLOSION :
          return 256;
        case W_SHELL :
          return 16;
        default :
          return 32;
      }
    };
    int getHeight() {
      switch (subtype) {
        case W_CLUB :
          return 16;
        case W_REVOLVER :
          return max(getWidth() / 2, 8);
        case W_SNIPER :
        case W_SHELL :
        case W_LASER :
          return 8;
        case W_DISK :
        case W_MINE :
          return 16;
        case W_ROCKET :
          return 24;
        default :
          return getWidth();
          break;
      }
    };
    int getGravity() {
      switch (subtype) {
        case W_GRENADE :
        case W_MINE :
        case W_SHELL :
          return 5;
        case W_CLUB :
        case W_DISK :
        case W_LASER :
        case W_ROCKET :
        case W_EXPLOSION :
          return 0;
        default:
          return abs(vx) > 16 ? 0 : 2;
      }
    };
    int getMaxSpeed() {
      return 128;
    }
    virtual int getXFriction() {
      switch (subtype) {
        case W_SHOTGUN :
          return 10;
        case W_CLUB :
        case W_DISK :
        case W_LASER :
          return 0;
        case W_ROCKET :
          return -20; //negative value so it accelerates
        case W_EXPLOSION :
          return 100;
        default :
          return Box::getXFriction();
      }
    }
    int getXBounce() {
      switch (subtype) {
        case W_DISK :
          return 100;
        case W_CLUB :
        case W_EXPLOSION :
        case W_LASER :
          return -1; //don't collide the world
        case W_GRENADE :
        case W_SHELL :
          return 80;
        case W_ROCKET :
          return 0;
        default :
          return 30;
      }
    };
    int getYBounce() {
      if ((subtype == W_GRENADE) || (subtype == W_SHELL))
        return getXBounce();
      else
        return 0;
    };

    int getDamage() {
      switch (subtype) {
          return 1;
        case W_REVOLVER :
        case W_CLUB :
          return 2;
        case W_SNIPER :
        case W_DISK :
        case W_EXPLOSION :
        case W_LASER :
          return 10;
        case W_GRENADE :
        case W_ROCKET :
        case W_MINE :
        case W_SHELL :
          return 0;
        default :
          return 1;
      }
    }

    int getMaxTimeLeft() {
      switch (subtype) {
        case W_CLUB :
          return 2;
        case W_SHELL :
          return 20;
        case W_MINE :
        case W_DISK :
          return 100;
        case W_EXPLOSION :
          return 5;
        case W_GRENADE :
        case W_ROCKET :
          return 40;
        default :
          return 25;
      }

    }

    boolean explodes() {
      switch (subtype) {
        case W_GRENADE :
        case W_ROCKET :
        case W_MINE :
          return true;
        default:
          return false;
      }
    }

    boolean destroyOnWorldContact() {
      return (subtype == W_ROCKET) ? true : false;
    }

    boolean destroyOnEnemyContact() {
      switch (subtype) {
        case W_CLUB :
        case W_DISK :
        case W_LASER :
        case W_EXPLOSION :
        case W_SHELL :
          return false;
        default:
          return true;
      }
    }

    boolean damagePlayer() {
      return (subtype == W_DISK) ? true : false;
    }

    void update() {
      if (timeLeft) {
        byte collided = Box::update();
        timeLeft--;
        if (destroyOnWorldContact() && collided == 1) {
          timeLeft = 0;
        }
        if ((timeLeft == 0) && explodes()) {
          subtype = W_EXPLOSION;
          x -= getWidth() / 2; //offset the explosion for it to be centered
          y -= getHeight() / 2;
          timeLeft = 8;
          //set the camera shake
          shakeTimeLeft = 10;
          shakeAmplitude = 2;
          //gb.sound.playPattern(blast_sound, 0);
          gb.sound.playTick();
        }
      }
    }

    void draw() {
      if (timeLeft) {
        if (subtype == W_LASER) {
          //gb.display.setColor(INVERT);
        }
        Box::draw();
      }
    }
};

#define NUMBULLETS 20
static Bullet bullets[NUMBULLETS];

/////////////////////////////////////////////
///////////////////////////////////////////// WEAPON
/////////////////////////////////////////////

const byte club[] PROGMEM = {
  24, 6, 0xC0, 0x0, 0x0, 0xF0, 0x0, 0x0, 0x7C, 0x0, 0x0, 0x1F, 0x0, 0x0, 0x7, 0x80, 0x0, 0x1, 0x80, 0x0,
};
const byte pistol[] PROGMEM = {
  24, 3, 0x0, 0x0, 0xF0, 0x0, 0x1, 0xE0, 0x0, 0x1, 0x0
};
const byte laser[] PROGMEM = {
  24, 5, 0x0, 0x0, 0x80, 0x0, 0xFF, 0xA8, 0x0, 0xFF, 0xFC, 0x0, 0xFF, 0xA8, 0x0, 0x0, 0x80,
};
const byte revolver[] PROGMEM = {
  24, 4, 0x0, 0x0, 0x80, 0x0, 0x0, 0xF8, 0x0, 0x1, 0xF8, 0x0, 0x1, 0xC0
};
const byte rifle[] PROGMEM = {
  24, 4, 0x0, 0x0, 0x4, 0x0, 0x21, 0xFC, 0x0, 0x37, 0xF0, 0x0, 0x31, 0x0
};
const byte rifle_white[] PROGMEM = {
  24, 3, 0x0, 0x0, 0x0, 0x0, 0x1E, 0x0, 0x0, 0x8, 0x0
};
const byte sniper[] PROGMEM = {
  24, 4, 0x0, 0x0, 0xC0, 0x0, 0xFF, 0xFE, 0x0, 0xFF, 0xE0, 0x0, 0xE3, 0x0,
};
const byte shotgun[] PROGMEM = {
  24, 4, 0x0, 0x0, 0x10, 0x0, 0x63, 0xF0, 0x0, 0x7F, 0xF0, 0x0, 0x7C, 0x0
};
const byte shotgun_white[] PROGMEM = {
  24, 2, 0x0, 0x0, 0x0, 0x0, 0x1C, 0x0
};
const byte machinegun[] PROGMEM = {
  24, 5, 0x0, 0x0, 0x10, 0x0, 0xC7, 0xF0, 0x0, 0xC7, 0xF0, 0x0, 0xC7, 0xC0, 0x0, 0xFC, 0x0,
};
const byte machinegun_white[] PROGMEM = {
  24, 4, 0x0, 0x0, 0x0, 0x0, 0x38, 0x0, 0x0, 0x38, 0x0, 0x0, 0x38, 0x0
};
const byte disk[] PROGMEM = {
  24, 5, 0x0, 0x0, 0x20, 0x0, 0xFF, 0xF0, 0x0, 0xFC, 0x0, 0x0, 0xFC, 0x0, 0x0, 0xFF, 0xF0,
};
const byte rocket[] PROGMEM = {
  24, 5, 0x1, 0x0, 0x20, 0x1, 0xFF, 0xE0, 0x1, 0xD7, 0xE0, 0x1, 0xFF, 0xE0, 0x1, 0x0, 0x20
};
const byte grenade[] PROGMEM = {
  24, 4, 0x0, 0xC7, 0xE0, 0x0, 0xFF, 0xE0, 0x0, 0xC7, 0xE0, 0x0, 0x7C, 0x0
};
const byte grenade_white[] PROGMEM = {
  24, 3, 0x0, 0x38, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0x0
};


class Weapon {
  public:
    byte subtype;
    byte cooldown;
    Box* shooter;

    void init() {
      cooldown = 0;
      for (byte i = 0; i < NUMBULLETS; i++) {
        bullets[i].timeLeft = 0;
      }
    }

    byte getMaxCooldown() {
      switch (subtype) {
        case W_CLUB :
          return 10;
        case W_PISTOL :
        case W_AKIMBO :
        case W_REVOLVER :
          return 0;
        case W_SNIPER :
          return 7;
        case W_SHOTGUN :
          return 11;
        case W_RIFLE :
          return 2;
        case W_MACHINEGUN :
          return 1;
        case W_DISK :
          return 19;
        case W_LASER :
          return 30;
        case W_GRENADE :
        case W_ROCKET :
        case W_MINE :
          return 19;
        case W_EXPLOSION :
        default :
          return 5;
      }
    }

    void addBullet(int x, int y, int dir, byte subtype) {
      for (byte i = 0; i < NUMBULLETS; i++) {
        if (bullets[i].timeLeft == 0) {
          bullets[i].subtype = subtype;
          bullets[i].timeLeft = bullets[i].getMaxTimeLeft();

          //screen shaking
          switch (subtype) {
            case W_SNIPER :
            case W_REVOLVER :
              shakeTimeLeft = 4;
              shakeAmplitude = 4;
            case W_MACHINEGUN :
              shakeTimeLeft = 2;
              shakeAmplitude = 1;
          }

          //initial bullet speeds
          switch (subtype) {
            case W_CLUB :
              bullets[i].vx = dir * 32;
              bullets[i].vy = shooter->vy;
              break;
            case W_MACHINEGUN :
              bullets[i].vx = (dir * 48) + random(-8, 9);
              bullets[i].vy = random(-16, 17);
              shooter->vx -= shooter->dir * 32; //player recoil
            case W_SHOTGUN :
              bullets[i].vx = (dir * 48) + random(-8, 9);
              bullets[i].vy = random(-10, 11);
              break;
            case W_DISK :
              bullets[i].vx = dir * 26;
              bullets[i].vy = 0;
              break;
            case W_LASER :
              bullets[i].vx = dir * 50;
              bullets[i].vy = 0;
              break;
            case W_GRENADE : //grenade speed depends on player's speed
              bullets[i].vx = (dir * 32) + shooter->vx / 2;
              bullets[i].vy = -32 + shooter->vy / 2;
              break;
            case W_ROCKET :
              bullets[i].vx = dir * 16;
              bullets[i].vy = 0;
              break;
            case W_MINE :
              bullets[i].vx = 0;
              bullets[i].vy = 0;
              break;
            case W_SHELL :
              bullets[i].vx = - dir * random(16, 24);
              bullets[i].vy = shooter->vy - random(16, 24);
              break;
            default :
              bullets[i].vx = (dir * 64) + random(-8, 9);
              bullets[i].vy = random(0, 11) - 5;
          }

          //bullet vertical offset
          switch (subtype) {
            case W_SHOTGUN :
            case W_LASER :
            case W_DISK :
              y += 32;
              break;
            case W_ROCKET :
              y += 16;
              break;
            default :
              y += 24;
              break;
          }

          //horizontal bullet offset
          switch (subtype) {
            case W_SHELL :
              x -= 16;
            case W_ROCKET:
            case W_CLUB :
              x -= dir * 32;
            case W_MINE :
              break;
            default :
              x += dir * 46;
          }

          if (dir > 0) {
            x += shooter->getWidth();
          }
          else {
            x -= bullets[i].getWidth();
          }

          x += shooter->vx / 2;
          bullets[i].x = x;
          bullets[i].y = y;

          break;
        }
      }
    };

    void shoot() {
      cooldown = getMaxCooldown();
      addBullet(shooter->x, shooter->y, shooter->dir, subtype);
      if (subtype == W_SHOTGUN) {
        addBullet(shooter->x, shooter->y, shooter->dir, subtype);
        addBullet(shooter->x, shooter->y, shooter->dir, subtype);
        addBullet(shooter->x, shooter->y, shooter->dir, subtype);
        addBullet(shooter->x, shooter->y, shooter->dir, subtype);
      }
      if (subtype == W_AKIMBO) {
        addBullet(shooter->x, shooter->y, -shooter->dir, subtype);
      }
      switch (subtype) { //eject a shell
        case W_RIFLE:
        case W_SNIPER:
        case W_SHOTGUN:
          addBullet(shooter->x, shooter->y, shooter->dir, W_SHELL);
      }

      switch (subtype) {
        case W_ROCKET :
          //gb.sound.playPattern(rocket_sound, 0);
          gb.sound.playTick();
          break;
        case W_REVOLVER :
        case W_MACHINEGUN :
        case W_SNIPER :
          //gb.sound.playPattern(machinegun_sound, 0);
          gb.sound.playTick();
          break;
        case W_GRENADE :
        case W_DISK :
          //gb.sound.playPattern(grenade_sound, 0);
          gb.sound.playTick();
          break;
        case W_SHOTGUN :
          //gb.sound.playPattern(shotgun_sound, 0);
          gb.sound.playTick();
          for(uint8_t i = 0; i < gb.neoPixels.numPixels(); i++){
            gb.neoPixels.setPixelColor(i, 255, 255, 128);
          }
          break;
        case W_MINE :
          break;
        case W_PISTOL :
        case W_AKIMBO :
        case W_RIFLE :
          gb.sound.playTick();
          break;
        case W_LASER :
          //gb.sound.playPattern(laser_sound, 0);
          gb.sound.playTick();
          break;
        case W_CLUB :
          //gb.sound.playPattern(club_sound, 0);
          gb.sound.playTick();
          break;
      }
    }

    boolean isAutomatic() {
      switch (subtype) {
        case W_RIFLE :
        case W_MACHINEGUN :
          return true;
        default :
          return false;
      }
    };

    void update() {
      for (byte i = 0; i < NUMBULLETS; i++) {
        bullets[i].update();
      }
      if (cooldown > 0) {
        cooldown--;
      }
      else {
        if (isAutomatic()) {
          if (gb.buttons.repeat(BTN_A, 1)) {
            shoot();
          }
        }
        else {
          if (gb.buttons.pressed(BTN_A)) {
            shoot();
          }
        }
      }
    }

    void draw() {
      int bx = toScreenX(shooter->x) - 9;
      int by = toScreenY(shooter->y);
      byte flip = (shooter->dir > 0) ? NOFLIP : FLIPH;
      const byte* bitmap;
      const byte* bitmapWhite;
      switch (subtype) {
        case W_CLUB :
          if (cooldown > 8) {
            bitmap = 0; //don't draw the club when already in use
          }
          else {
            bitmap = club;
            bx += shooter->dir * cooldown / 2; //sliding back the club
          }
          bitmapWhite = 0;
          by -= 2;
          break;
        case W_PISTOL :
        case W_AKIMBO :
          bitmap = pistol;
          bitmapWhite = 0;
          by += 3;
          break;
        case W_REVOLVER :
          bitmap = revolver;
          bitmapWhite = 0;
          by += 2;
          break;
        case W_SNIPER :
          bitmap = sniper;
          bitmapWhite = shotgun_white;
          by += 2;
          bx -= (cooldown > 4) ? shooter->dir : 0;
          break;
        case W_RIFLE :
          bitmap = rifle;
          bitmapWhite = rifle_white;
          by += 2;
          bx -= shooter->dir * cooldown;
          break;
        case W_SHOTGUN :
          bitmap = shotgun;
          bitmapWhite = shotgun_white;
          by += 3;
          bx -= shooter->dir * cooldown / 4;
          break;
        case W_MACHINEGUN :
          bitmap = machinegun;
          bitmapWhite = machinegun_white;
          by += 3;
          break;
        case W_DISK :
          bitmap = disk;
          bitmapWhite = 0;
          by += 2;
          break;
        case W_LASER :
          bitmap = laser;
          bitmapWhite = 0;
          by += 2;
          break;
        case W_GRENADE :
          bitmap = grenade;
          bitmapWhite = grenade_white;
          by += 4;
          break;
        case W_ROCKET :
          bitmap = rocket;
          bitmapWhite = 0;
          by += 1;
          break;
        default :
          bitmap = 0;
          bitmapWhite = 0;
      }

      if (bitmap) { //draw the weappon
        gb.display.drawBitmap(bx, by, bitmap, NOROT, flip);
      }
      if (bitmapWhite) {
        gb.display.setColor(WHITE);
        gb.display.drawBitmap(bx, by, bitmapWhite, NOROT, flip);
        gb.display.setColor(BLACK);
      }
      if (subtype == W_AKIMBO) { //draw the symetric of the pistol in the akimbo case
        if (bitmap) {
          gb.display.drawBitmap(bx, by, bitmap, NOROT, (flip + 1) % 2);
        }
        if (bitmapWhite) {
          gb.display.setColor(WHITE);
          gb.display.drawBitmap(bx, by, bitmapWhite, NOROT, (flip + 1) % 2);
          gb.display.setColor(BLACK);
        }
      }
      if (subtype == W_LASER) { //reloading line on the laser
        gb.display.setColor(WHITE);
        gb.display.drawFastHLine(toScreenX(shooter->x), toScreenY(shooter->y) + 4, 6 - cooldown / 5);
        gb.display.setColor(BLACK);
      }
      if ((subtype == W_DISK) || (subtype == W_MINE)) { //refill animation
        if (shooter->dir > 0) {
          gb.display.fillRect(toScreenX(shooter->x) + 6, toScreenY(shooter->y) + 4, 4 - cooldown / 4, 2);
        }
        else {
          gb.display.fillRect(toScreenX(shooter->x) + cooldown / 4 - 4, toScreenY(shooter->y) + 4, 4, 2);
        }
      }
    }
};

/////////////////////////////////////////////
///////////////////////////////////////////// PLAYER
/////////////////////////////////////////////

const byte playerBitmap[][11] PROGMEM = {
  {8, 9, 0x0, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x66,},
  {8, 9, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x3C, 0x3C, 0xC,},
  {8, 9, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x38, 0x38, 0x18,},
  {8, 9, 0x0, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x1C, 0x1C, 0x18,},
  {8, 9, 0x0, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x3C, 0x30,}
};


class Player :
  public Box {
  public:
    Weapon weapon;
    boolean jumping;
    boolean doubleJumped;
    boolean dead;
    unsigned int score;

    void init() {
      x = 128;
      y = 150;
      dir = 1;
      score = 0;
      dead = false;
      weapon.init();
      weapon.shooter = this;
    }

    int getWidth() {
      return 48;
    }
    int getHeight() {
      return 72;
    }
    int getGravity() {
      return 8;
    }
    int getXFriction() {
      if (dead) {
        return 10;
      }
      else {
        return 40;
      };
    }
    int getXBounce() {
      if (dead) {
        return -1;
      }
      else {
        return 0;
      };
    }
    int getYBounce() {
      return 0;
    }

    void kill(int dir) {
      dead = true;
      vx = dir * 32;
      vy = -32;
      popupTimeLeft = 0;
      if (world.addScore(score)) {
        popup("NEW HIGHSCORE!", 40);
      }
      saveEEPROM();
    }

    void update() {
      if (!dead) {
        //player input
        if (gb.buttons.repeat(BTN_LEFT, 1)) {
          dir = -1;
          vx -= 16;
        }
        if (gb.buttons.repeat(BTN_RIGHT, 1)) {
          dir = 1;
          vx += 16;
        }
        if (gb.buttons.repeat(BTN_UP, 10) && (gb.buttons.timeHeld(BTN_DOWN) > 10)) {
          weapon.subtype ++;
          weapon.subtype %= NUMWEAPONS;
          score = 0;
          popup("WEAPON CHEAT");
        }
        if (gb.buttons.repeat(BTN_LEFT, 10) && (gb.buttons.timeHeld(BTN_RIGHT) > 10)) {
          unlockedMaps = NUMMAPS - 1;
          unlockedWeapons = NUMWEAPONS - 1;
          score = 0;
          popup("ALL UNLOCKED");
        }

        if (y > world.getHeight()) {
          kill(dir);
        }

        //jumping
        if (world.solidCollisionAtPosition(x, y + 1, getWidth(), getHeight())) {
          doubleJumped = false;
        }
        if (gb.buttons.pressed(BTN_B)) {
          if (world.solidCollisionAtPosition(x, y + 1, getWidth(), getHeight())) {
            vy = -32;
            jumping = true;
            //gb.sound.playPattern(jump_sound, 1);
            gb.sound.playTick();
          }
          else {
            if (doubleJumped == false) {
              vy = -32;
              doubleJumped = true;
              jumping = true;
              //gb.sound.playPattern(jump_sound, 1);
              gb.sound.playTick();
            }
          }
        }
        if ((gb.buttons.timeHeld(BTN_B) > 0) && (gb.buttons.timeHeld(BTN_B) < 5) && (vy < 0) && jumping) {
          if (doubleJumped) {
            vy -= 6;
          }
          else {
            vy -= 12;
          }
        }
        if (vy > 0) {
          jumping = false;
        }
      }

      weapon.update();

      int d = dir;
      Box::update(); //update physics
      dir = d; //override the direction calculated by Box::update(), which depend on velocity.
      //here the direction of the player should only depend on user input
      //I know, it's not very clean to do that this way
    }

    void draw() {
      if (isOffScreen()) {
        return;
      }
      byte flip = (dir > 0) ? NOFLIP : FLIPH;
      byte frame = (dir * x / 32 + 255) % 5; //get the current frame from the x position. Add 255 to avoid being under 0.
      if (vx == 0) {
        frame = 0;
      }
      if (!world.solidCollisionAtPosition(x, y + 1, getWidth(), getHeight())) { //in the air
        if (vy < 0) {
          frame = 4;
        }
        else {
          frame = 1;
        }
      }
      gb.display.drawBitmap(toScreenX(x) - 1, toScreenY(y), playerBitmap[frame], NOROT, flip);
      weapon.draw();
    }
};

Player player;

/////////////////////////////////////////////
///////////////////////////////////////////// ENEMY
/////////////////////////////////////////////

const byte smallEnemyBitmap[][10] PROGMEM = {
  { 8, 8, 0x0, 0x7E, 0x6C, 0x6C, 0x7E, 0x7E, 0x7E, 0x66,},
  { 8, 8, 0x7E, 0x6C, 0x6C, 0x7E, 0x7E, 0x3C, 0x3C, 0xC,},
  { 8, 8, 0x7E, 0x6C, 0x6C, 0x7E, 0x7E, 0x38, 0x38, 0x18,},
  { 8, 8, 0x7E, 0x6C, 0x6C, 0x7E, 0x7E, 0x1C, 0x1C, 0x18,},
  { 8, 8, 0x0, 0x7E, 0x6C, 0x6C, 0x7E, 0x7E, 0x3C, 0x30,}
};

const byte bigEnemyBitmap[][22] PROGMEM = {
  { 16, 10, 0x0, 0x0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3D, 0xA0, 0x3D, 0xA0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0x38, 0xE0, 0x38, 0xE0,},
  { 16, 10, 0x0, 0x0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3D, 0xA0, 0x3D, 0xA0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0x1, 0xC0,},
  { 16, 10, 0x3F, 0xE0, 0x3F, 0xE0, 0x3D, 0xA0, 0x3D, 0xA0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0xF, 0x0, 0x7, 0x0,},
  { 16, 10, 0x3F, 0xE0, 0x3F, 0xE0, 0x3D, 0xA0, 0x3D, 0xA0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0x7, 0x80, 0x7, 0x0,},
  { 16, 10, 0x3F, 0xE0, 0x3F, 0xE0, 0x3D, 0xA0, 0x3D, 0xA0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0xF, 0x80, 0xE, 0x0,},
  { 16, 10, 0x3F, 0xE0, 0x3F, 0xE0, 0x3D, 0xA0, 0x3D, 0xA0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0x3F, 0xE0, 0x1D, 0xC0, 0x1C, 0x0,}
};

class Enemy :
  public Box {
  public:
    byte subtype;
    boolean active;
    int health;
    int getWidth() {
      return (subtype == E_SMALL) ? 48 : 72;
    }
    int getHeight() {
      return (subtype == E_SMALL) ? 64 : 80;
    }
    int getGravity() {
      if (health > 0)
        return 4;
      else
        return 10;
    }
    int getXFriction() {
      return 0;
    }
    int getXBounce() {
      if (health > 0) {
        return 100;
      }
      else {
        return -1;
      }
    }
    int getYBounce() {
      return 0;
    }
    int getMaxHealth() {
      return (subtype == E_SMALL) ? 2 : 10;
    }
    int getMaxSpeed() {
      return 32;
    }

    int update() {
      if (active) {
        Box::update();
        //respawn in "angry" mod whel enemies falls at the bottom of the map
        if (y > world.getHeight()) {
          if (health > 0) {
            x = world.getWidth() / 2 - getWidth() / 2;
            y = 0;
            vx = dir * 20;
            //gb.sound.playPattern(enemy_felt_sound, 2);
          }
          else {
            active = false;
          }
        }
      }
    }
    void draw() {
      if (isOffScreen()) {
        return;
      }
      int flip = (dir > 0) ? NOFLIP : FLIPH;
      if (subtype == E_SMALL) {
        byte frame = (dir * x / 16 + 255) % 5; //get the current frame from the x position. Add 255 to avoid being under 0.
        gb.display.drawBitmap(toScreenX(x) - 1, toScreenY(y), smallEnemyBitmap[frame], NOROT, flip);
      }
      else {
        byte frame = (dir * x / 16 + 255) % 6; //get the current frame from the x position. Add 255 to avoid being under 0.
        gb.display.drawBitmap(toScreenX(x) - 4, toScreenY(y), bigEnemyBitmap[frame], NOROT, flip);
      }
    }
};

/////////////////////////////////////////////
///////////////////////////////////////////// ENEMY ENGINE
/////////////////////////////////////////////

#define NUMENEMIES 20
Enemy enemies[NUMENEMIES];

class EnemiesEngine {
  public:
    int nextSpawnCount;
    void init() {
      nextSpawnCount = 10;
      for (byte i = 0; i < NUMENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].health = 0;
      }
    }

    void addEnemy() {
      for (byte i = 0; i < NUMENEMIES; i++) {
        if (!enemies[i].active) {
          enemies[i].active = true;
          if ((random(0, 6) == 0) && (world.mapNumber != 0)) { //randomly spawn a few big monsters
            enemies[i].subtype = E_BIG;
          }
          else {
            enemies[i].subtype = E_SMALL;
          }
          enemies[i].health = enemies[i].getMaxHealth();
          enemies[i].x = world.getWidth() / 2 - enemies[i].getWidth() / 2;
          enemies[i].y = 0;
          enemies[i].vx = (random(0, 2) * 20) - 10;
          enemies[i].vy = 0;
          return;
        }
      }
    }

    void update() {
      for (byte j = 0; j < NUMBULLETS; j++) {
        //skip inactive bullets
        if (bullets[j].timeLeft <= 0) {
          continue;
        }
        for (byte i = 0; i < NUMENEMIES; i++) {
          //skip dead enemies
          if (enemies[i].health <= 0) {
            continue;
          }
          //skip bullets with a low speed (falling particles) except explosions, mines and grenades
          if ((abs(bullets[j].vx) < 20) && !((bullets[j].subtype == W_EXPLOSION) || (bullets[j].subtype == W_MINE) || (bullets[j].subtype == W_GRENADE))) {
            break;
          }
          if (gb.collideRectRect(enemies[i].x, enemies[i].y, enemies[i].getWidth(), enemies[i].getHeight(),
                                 bullets[j].x, bullets[j].y, bullets[j].getWidth(), bullets[j].getHeight())) {
            if (bullets[j].explodes()) {
              bullets[j].timeLeft = 1;
            }
            if (bullets[j].destroyOnEnemyContact()) {
              bullets[j].vx = (bullets[j].vx * bullets[j].getXBounce()) / 100;
            }
            enemies[i].health -= bullets[j].getDamage();
            //make the ennemy jump when dead
            if (enemies[i].health <= 0) {
              int dir;
              if (bullets[j].subtype == W_EXPLOSION) { //fly away from the explosive
                dir = (((enemies[i].x + enemies[i].getWidth() / 2) - (bullets[j].x + bullets[j].getWidth() / 2)) > 0) ? 1 : -1;
              }
              else { //fly in the same direction that the incoming bullet
                dir = bullets[j].vx > 0 ? 1 : -1;
              }
              //throw the enemy in the air
              enemies[i].vx = dir * random(24, 32);
              enemies[i].vy = random(-48, -64);
              //gb.sound.playPattern(enemy_death_sound, 1);
              gb.sound.playCancel();
            }
            else {
              if (bullets[j].subtype == W_CLUB) { // if not dead, go away from the player when hit by a club
                int dir = (enemies[i].x + enemies[i].getWidth() / 2) - (player.x + player.getWidth() / 2) > 0 ? 1 : -1;
                enemies[i].vx = dir * abs(enemies[i].vx);
              }
            }
          }
        }
      }
      nextSpawnCount--;
      if (!nextSpawnCount) { //spawn enemies
        //spawn rate increase slowly depending on score
        //when score is 0, monsters spawn every 60 frames (3 s)
        //when score is 50 monsters swpan every 30 frames (1.5 s)
        //they won't spaw faster than every 10 frames (0.5 s)
        nextSpawnCount = map(player.score, 0, 50, 60, 30);
        nextSpawnCount = max(nextSpawnCount, 10);
        addEnemy();
      }
      for (byte i = 0; i < NUMENEMIES; i++) {
        enemies[i].update();
      }

    }

    void draw() {
      for (byte i = 0; i < NUMENEMIES; i++) {
        if (enemies[i].active) {
          if((abs(enemies[i].vx) > 10) && (enemies[i].health > 0)) //angry enemy
            gb.display.setColor(RED);
          enemies[i].draw();
          gb.display.setColor(BLACK);
        }
      }
    }

};

EnemiesEngine enemiesEngine;


///////////////////////////////////////////// CRATE
const byte crateBitmap[] PROGMEM = {
  8, 6, 0xFC, 0x84, 0xEC, 0xDC, 0x84, 0xFC,
};

class Crate :
  public Box {
  public:
    int getWidth() {
      return 48;
    }
    int getHeight() {
      return 48;
    }

    void init() {
      vy = 0;
      boolean goodSpot;
      do { //pick a random location somewhere in the world (distance from the border = 1 sprite max)
        x = random(SPRITE_SIZE * SCALE , world.getWidth() - SPRITE_SIZE * SCALE - getWidth());
        y = random(SPRITE_SIZE * SCALE, world.getHeight() - SPRITE_SIZE * SCALE - getHeight());
        goodSpot = true;
        if ((abs(player.x - x) < 128) || (abs(player.y - y) < 128))
          goodSpot = false; //too close to the player
        if ((x > (world.getWidth() / 2 - 128)) && (x < (world.getWidth() / 2 + 128)) && (y < 336))
          goodSpot = false; //avoid the top central zone where mobs spawn
      }
      while (!goodSpot);
    }

    void update() {
      Box::update();
      //reinit the crate if it felt out of the world
      if (y > world.getHeight()) {
        init();
      }
      //pick random weapon when taken by the player
      if (gb.collideRectRect(x, y, getWidth(), getHeight(),
                             player.x, player.y, player.getWidth(), player.getHeight())) {
        player.score++;
        gb.sound.playOK();
        //add a random value to the weapon type inferior to the number of weapons
        //to avoid picking the same weapon
        player.weapon.subtype = (player.weapon.subtype + random(1, unlockedWeapons + 1)) % (unlockedWeapons + 1);
        switch (player.weapon.subtype) {
          case W_CLUB :
            popup("CLUB");
            break;
          case W_PISTOL :
            popup("PISTOL");
            break;
          case W_AKIMBO :
            popup("AKIMBO");
            break;
          case W_REVOLVER :
            popup("REVOLVER");
            break;
          case W_SNIPER :
            popup("SNIPER");
            break;
          case W_SHOTGUN :
            popup("SHOTGUN");
            break;
          case W_RIFLE :
            popup("RIFLE");
            break;
          case W_MACHINEGUN :
            popup("MACHINEGUN");
            break;
          case W_DISK :
            popup("DISK");
            break;
          case W_LASER :
            popup("LASER");
            break;
          case W_GRENADE :
            popup("GRENADE");
            break;
          case W_ROCKET :
            popup("ROCKET");
            break;
          case W_MINE :
            popup("MINE");
            break;
        }
        if (world.mapNumber == 0) {
          switch (player.score) {
            case (SCORETHRESHOLD_1):
              if (unlockedWeapons < W_RIFLE) {
                unlockedWeapons = W_RIFLE;
                player.weapon.subtype = W_RIFLE;
                popup("RIFLE UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 2);
              }
              break;
            case (SCORETHRESHOLD_2):
              if (unlockedWeapons < W_SHOTGUN) {
                unlockedWeapons = W_SHOTGUN;
                player.weapon.subtype = W_SHOTGUN;
                popup("SHOTGUN UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 2);
              }
              break;
            case (SCORETHRESHOLD_3):
              if (unlockedMaps < 1) {
                unlockedMaps = 1;
                popup("NEW MAP UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
          }
        }
        if (world.mapNumber == 1) {
          switch (player.score) {
            case (SCORETHRESHOLD_1):
              if (unlockedWeapons < W_ROCKET) {
                unlockedWeapons = W_ROCKET;
                player.weapon.subtype = W_ROCKET;
                popup("ROCKETS UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
            case (SCORETHRESHOLD_2):
              if (unlockedWeapons < W_CLUB) {
                unlockedWeapons = W_CLUB;
                player.weapon.subtype = W_CLUB;
                popup("CLUB UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
            case (SCORETHRESHOLD_3):
              if (unlockedWeapons < W_REVOLVER) {
                unlockedWeapons = W_REVOLVER;
                player.weapon.subtype = W_REVOLVER;
                popup("REVOLVER UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
            case (SCORETHRESHOLD_4):
              if (unlockedWeapons < W_MINE) {
                unlockedWeapons = W_MINE;
                player.weapon.subtype = W_MINE;
                popup("MINES UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
            case (SCORETHRESHOLD_5):
              if (unlockedMaps < 2) {
                unlockedMaps = 2;
                popup("NEW MAP UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
          }
        }
        if (world.mapNumber == 2) {
          switch (player.score) {
            case (SCORETHRESHOLD_1):
              if (unlockedWeapons < W_SNIPER) {
                unlockedWeapons = W_SNIPER;
                player.weapon.subtype = W_SNIPER;
                popup("SNIPER UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
            case (SCORETHRESHOLD_2):
              if (unlockedWeapons < W_MACHINEGUN) {
                unlockedWeapons = W_MACHINEGUN;
                player.weapon.subtype = W_MACHINEGUN;
                popup("MACHINEGUN UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
            case (SCORETHRESHOLD_3):
              if (unlockedWeapons < W_GRENADE) {
                unlockedWeapons = W_GRENADE;
                player.weapon.subtype = W_GRENADE;
                popup("GRENADES UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
            case (SCORETHRESHOLD_4):
              if (unlockedWeapons < W_AKIMBO) {
                unlockedWeapons = W_AKIMBO;
                player.weapon.subtype = W_AKIMBO;
                popup("AKIMBO UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
            case (SCORETHRESHOLD_5):
              if (unlockedMaps < 3) {
                unlockedMaps = 3;
                popup("NEW MAP UNLOCKED!", 40);
                //gb.sound.playPattern(power_up_sound, 0);
              }
              break;
          }
        }
        if (world.mapNumber == 3) {
          switch (player.score) {
            case (SCORETHRESHOLD_3):
              if (unlockedWeapons < W_DISK) {
                unlockedWeapons = W_DISK;
                player.weapon.subtype = W_DISK;
                popup("DISK UNLOCKED!", 40);
              }
              break;
            case (SCORETHRESHOLD_4):
              if (unlockedWeapons < W_LASER) {
                unlockedWeapons = W_LASER;
                player.weapon.subtype = W_LASER;
                popup("LASER UNLOCKED!", 40);
              }
              break;
            case (SCORETHRESHOLD_5):
              if (unlockedMaps < 4) {
                unlockedMaps = 4;
                popup("LAST MAP UNLOCKED!", 40);
              }
              break;
          }
        }
        init(); //move the crate
      }
    }

    void draw() {
      if (isOffScreen())
        return;
      gb.display.setColor(0xFD03);
      gb.display.fillRect(toScreenX(x), toScreenY(y), 6, 6);
      gb.display.setColor(0xAA86);
      gb.display.drawBitmap(toScreenX(x), toScreenY(y), crateBitmap);
      gb.display.setColor(BLACK);
    }
};

Crate crate;

///////////////////////////////////////////// SETUP
void setup() {
  gb.begin();
  loadEEPROM();
  //gb.sound.chanVolumes[2] = 1;
  mainMenu();
  world.chooseMap();
}

///////////////////////////////////////////// LOOP
void loop() {
  if (gb.update()) {
    if (gb.buttons.pressed(BTN_C)) {
      gamePaused();
    }

    crate.update();
    player.update();
    enemiesEngine.update();
    saveEEPROM(); //it checks if the values have changed before writting so it won't wear out the EEPROM

    //camera smoothing
    //int x = (player.x + player.getWidth()/2)/SCALE - LCDWIDTH/2;
    //int y = (player.y + player.getHeight()/2)/SCALE - LCDHEIGHT/2;
    //cameraX = (3*cameraX + x)/4;
    //cameraY = (3*cameraY + y)/4;

    //update camera
    if ((world.getWidth()/SCALE) <= LCDWIDTH) {
      cameraX = 0;
    } else {
      cameraX = (player.x + player.getWidth() / 2) / SCALE - LCDWIDTH / 2;
      cameraX = constrain(cameraX, 0, world.getWidth() / SCALE - LCDWIDTH);
    }
    if ((world.getHeight()/SCALE) <= LCDHEIGHT) {
      cameraY = 0;
    } else {
      cameraY = (player.y + player.getHeight() / 2) / SCALE - LCDHEIGHT / 2;
      cameraY = constrain(cameraY, 0, (world.getHeight() / SCALE) - LCDHEIGHT);
    }

    if (shakeTimeLeft > 0) {
      shakeTimeLeft --;
      cameraX += random(- 1, 2) * shakeAmplitude;
      cameraY += random(- 1, 2) * shakeAmplitude;
    }

    drawAll();

    //check for player - monsters collisions
    for (byte i = 0; i < NUMENEMIES; i++) {
      if (gb.collideRectRect(enemies[i].x, enemies[i].y, enemies[i].getWidth(), enemies[i].getHeight(),
                             player.x, player.y, player.getWidth(), player.getHeight())) {
        if (enemies[i].health > 0) {
          player.dead = true;
          int dir = (((enemies[i].x + enemies[i].getWidth() / 2) - (player.x + player.getWidth() / 2)) > 0) ? -1 : 1;
          player.kill(dir);
          break;
        }
      }
    }
    //check for player - bullet collisons
    for (byte i = 0; i < NUMBULLETS; i++) {
      //skip inactive bullets
      if (bullets[i].timeLeft <= 0) {
        continue;
      }
      if (bullets[i].damagePlayer() && gb.collideRectRect(bullets[i].x, bullets[i].y, bullets[i].getWidth(), bullets[i].getHeight(),
          player.x, player.y, player.getWidth(), player.getHeight())) {
        player.kill(bullets[i].dir);
      }
    }

    if (player.dead) {
      byte count = 20;
      if (!popupTimeLeft) { //if the "new highscore" popup is no here
        popup("GAME OVER!", 20);
      }
      while (1) {
        if (gb.update()) {
          player.update();
          enemiesEngine.update();
          drawAll();
          count --;
          if (!count) {
            break;
          }
          if (gb.buttons.pressed(BTN_C)) {
            break;
          }
        }
      }
      initGame();
    }
  }
}

void mainMenu() {
  gb.titleScreen(logo);
  gb.pickRandomSeed();
  //gb.battery.show = false;
}

void gamePaused() {
  while (1) {
    if (gb.update()) {
      drawAll();
      gb.display.setColor(BLACK, WHITE);
      gb.display.cursorX = 0;
      gb.display.cursorY = 0;
      gb.display.println("GAME PAUSED");
      gb.display.println("B: SAVE & QUIT");
      gb.display.println("C: RESUME");

      if (gb.buttons.pressed(BTN_C)) {
        return;
      }
      if (gb.buttons.pressed(BTN_B)) {
        world.addScore(player.score);
        saveEEPROM();
        world.chooseMap();
        return;
      }
    }
  }
}

void initGame() {
  player.init();
  enemiesEngine.init();
  crate.init();
  player.weapon.subtype = 0;
  shakeTimeLeft = 0;
}

void drawAll() {
  world.draw();
  crate.draw();
  enemiesEngine.draw();
  player.draw();
  if (!player.dead && world.mapNumber) {
    drawCompass();
  }
  //draw explosions
  for (byte i = 0; i < NUMBULLETS; i++) {
    if (bullets[i].subtype == W_EXPLOSION) {
      //gb.display.setColor(INVERT);
    }
    bullets[i].draw();
    gb.display.setColor(BLACK);
  }
  gb.display.setColor(BLACK, WHITE);
  gb.display.println(player.score);
  updatePopup();
  /*gb.display.cursorX = 0;
  gb.display.cursorY = 8;
  gb.display.setColor(0x00FF);
  gb.display.println(world.getHeight()*SCALE);
  gb.display.setColor(BLACK,WHITE);*/
}

void loadEEPROM() {
  /*if (EEPROMreadInt(0) != EEPROM_TOKEN) {
    cleanEEPROM();
    return;
  }
  //load score for each map
  for (int i = 0; i < NUMMAPS; i++) {
    score[i] = EEPROMreadInt(i * 2 + EEPROM_SCORE_OFFSET);
  }
  unlockedWeapons = EEPROM.read(EEPROM_WEAPONS_OFFSET);
  unlockedMaps = EEPROM.read(EEPROM_MAPS_OFFSET);
  world.mapNumber = unlockedMaps; //select the last unlocked map by */
}

void saveEEPROM() {
  /*EEPROMwriteInt(0, EEPROM_TOKEN);
  //save score for each map
  for (byte i = 0; i < NUMMAPS; i++) {
    if (EEPROMreadInt(i * 2 + EEPROM_SCORE_OFFSET) < score[i]) {
      EEPROMwriteInt(i * 2 + EEPROM_SCORE_OFFSET, score[i]);
    }
  }
  if (EEPROM.read(EEPROM_WEAPONS_OFFSET) < unlockedWeapons) {
    EEPROM.write(EEPROM_WEAPONS_OFFSET, unlockedWeapons);
  }
  if (EEPROM.read(EEPROM_MAPS_OFFSET) < unlockedMaps) {
    EEPROM.write(EEPROM_MAPS_OFFSET, unlockedMaps);
  }*/
}

unsigned int EEPROMreadInt(unsigned int i) {
  /*int value = EEPROM.read(i + 1) & 0x00FF; //LSB
  value += (EEPROM.read(i) << 8) & 0xFF00; //MSB
  return value;*/
}

void EEPROMwriteInt(unsigned int i, unsigned int value) {
 /* EEPROM.write(i + 1, value & 0x00FF); //LSB
  EEPROM.write(i, (value >> 8) & 0x00FF); //MSB*/
}

void cleanEEPROM() {
  /*for (int i = 0; i < 1024; i++) {
    if (EEPROM.read(i))
      EEPROM.write(i, 0);
  }*/
}


void printCentered(const char* text) {
  gb.display.cursorX = (LCDWIDTH / 2) - (strlen(text) * gb.display.fontSize * gb.display.fontWidth / 2);
  //gb.display.cursorX = 5;
  gb.display.print(text);
}

void printCentered(char* text) {
  gb.display.cursorX = (LCDWIDTH / 2) - (strlen(text) * gb.display.fontSize * gb.display.fontWidth / 2);
  //gb.display.cursorX = 5;
  gb.display.print(text);
}

void popup(const char* text, uint8_t duration) {
  popupText = text;
  popupTimeLeft = duration + 12;
}

void popup(const char* text) {
  popup(text, 20);
}

void updatePopup() {
  if (popupTimeLeft) {
    uint8_t yOffset = 0;
    if (popupTimeLeft < 12) {
      yOffset = popupTimeLeft - 12;
    }
    byte width = strlen(popupText) * gb.display.fontSize * gb.display.fontWidth;
    //byte width = 70;
    gb.display.fontSize = 1;
    gb.display.setColor(BLACK);
    gb.display.drawRect(LCDWIDTH / 2 - width / 2 - 2, yOffset - 1, width + 2, gb.display.fontHeight + 2);
    gb.display.setColor(WHITE);
    gb.display.fillRect(LCDWIDTH / 2 - width / 2 - 1, yOffset - 1, width + 1, gb.display.fontHeight + 1);
    gb.display.setColor(BLACK);
    gb.display.cursorY = yOffset;
    printCentered(popupText);
    popupTimeLeft--;
  }
}

void drawCompass() {
  //if (crate.isOffScreen()) {
  int x = (crate.x + crate.getWidth() / 2 - player.x - player.getWidth() / 2) / SCALE;
  int y = (crate.y + crate.getWidth() / 2 - player.y - player.getHeight() / 2) / SCALE;
  int dist = sqrt(x * x + y * y);
  if (dist > 20  ) {
    int dx = toScreenX(player.x + player.getWidth() / 2)   + (16 * x / dist);
    int dy = toScreenY(player.y + player.getHeight() / 2)  + (16 * y / dist);
    //gb.display.setColor(GRAY);
    gb.display.setColor(0xFD03);
    gb.display.drawLine(dx, dy, dx + x / 8, dy + y / 8);
    gb.display.setColor(BLACK);
  }
}


/*void adjustContrasts(){
  initGame();
  byte contrast = gb.display.contrast;
  while(1){
    if(gb.update()){
      if(gb.buttons.repeat(BTN_UP,2)){
        contrast ++;
        gb.display.setContrast(contrast);
      }
      if(gb.buttons.repeat(BTN_DOWN,2)){
        contrast --;
        gb.display.setContrast(contrast);
      }
      if(gb.buttons.pressed(BTN_A)){
        break;
      }
      if(gb.buttons.pressed(BTN_B)){
        break;
      }

      player.weapon.subtype = 7;
      if((gb.frameCount)%20 == 0){
        player.weapon.shoot();
      }
      player.weapon.update();
      drawAll();

      gb.display.cursorY = 0;
      gb.display.setColor(BLACK, WHITE);
      gb.display.print(F("Adjust contrasts\nfor this game:  "));
      gb.display.print(F("\37"));
      gb.display.print(contrast);
      gb.display.println(F("\36"));
      gb.display.cursorY = LCDHEIGHT - 5;
      gb.display.print(F("  A:OK B:Default"));
    }
  }
}*/