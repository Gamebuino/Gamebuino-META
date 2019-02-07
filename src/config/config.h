#ifndef _CONFIG_GAMEBUINO_META_
#define _CONFIG_GAMEBUINO_META_

// some common definitions that shouldn't be changed
#define SAVETYPE_INT 0
#define SAVETYPE_BLOB 1

#define DISPLAY_MODE_RGB565 0
#define DISPLAY_MODE_INDEX 1
#define DISPLAY_MODE_INDEX_HALFRES 2

// let's first include our config. First one can be overriden inside the sketch, config.h contains the default configs
#include <sketch/config-gamebuino.h>
#include "config-default.h"

#ifndef DISPLAY_CONSTRUCTOR
#if DISPLAY_MODE == DISPLAY_MODE_RGB565
#define DISPLAY_CONSTRUCTOR Image(80, 64, ColorMode::rgb565)
#endif
#if DISPLAY_MODE == DISPLAY_MODE_INDEX
#define DISPLAY_CONSTRUCTOR Image(160, 128, ColorMode::index)
#endif
#if DISPLAY_MODE == DISPLAY_MODE_INDEX_HALFRES
#define DISPLAY_CONSTRUCTOR Image(80, 64, ColorMode::index)
#endif
#endif // DISPLAY_CONSTRUCTOR

#if DISPLAY_MODE == DISPLAY_MODE_INDEX
#define SYSTEM_DEFAULT_FONT_SIZE 2
#else
#define SYSTEM_DEFAULT_FONT_SIZE 1
#endif

#if CUSTOM_MALLOC
#include <stddef.h>
extern "C" {
void* gb_malloc(size_t);
void gb_free(void*);
void* gb_memalign(size_t, size_t);
}
inline void* operator new(size_t size) {
	return gb_malloc(size);
}
inline void* operator new[](size_t size) {
	return gb_malloc(size);
}
inline void operator delete(void* ptr) {
	gb_free(ptr);
}
inline void operator delete[](void* ptr) {
	gb_free(ptr);
}
#else
#include <stdlib.h>
#include <malloc.h>
#define gb_malloc(x) malloc(x)
#define gb_free(x) free(x)
#define gb_memalign(x, y) memalign(x, y)
#endif

#if NO_ARDUINO
#include <sam.h>
#include <stdint.h>
#include <string.h>
extern "C" {
uint32_t millis(void);
uint32_t micros(void);
void delay(uint32_t);
void gamebuino_meta_pick_random_seed(void);
}
#else
#include <Arduino.h>
#endif

#ifndef constrain
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

#ifndef abs
#define abs(x) ((x)>0?(x):-(x))
#endif

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

#define SETTING_VOLUME 0
#define SETTING_VOLUME_MUTE 1
#define SETTING_DEFAULTNAME 2
#define SETTING_LANGUAGE 3
#define SETTING_NEOPIXELS_INTENSITY 4
#define SETTING_SETUP 5

#endif // _CONFIG_GAMEBUINO_META_
