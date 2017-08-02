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

#define SETTING_VOLUME 0
#define SETTING_VOLUME_MUTE 1
#define SETTING_DEFAULTNAME 2

#endif // _CONFIG_GAMEBUINO_META_
