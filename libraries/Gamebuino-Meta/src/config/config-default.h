#ifndef FOLDER_NAME
#ifndef __SKETCH_NAME__
#define FOLDER_NAME "testsketch"
#endif
#endif

///////////////
// GUI settings
///////////////

//enable menu, keyboard, pop-up, volume adjust functions
#ifndef ENABLE_GUI
#define ENABLE_GUI 1
#endif

//40 = 40 frames (2sec) before start menu is skipped, 0 = no start menu, 255 = start menu until you press A
#ifndef START_MENU_TIMER
#define START_MENU_TIMER 255
#endif

#ifndef KEYBOARD_W
#define KEYBOARD_W 16
#endif

#ifndef KEYBOARD_H
#define KEYBOARD_H 8
#endif

///////////////
// save defines
///////////////

#ifndef SAVEBLOCK_NUM
#define SAVEBLOCK_NUM 64
#endif

#ifndef SAVECONF
#define SAVECONF {}
#endif

#ifndef SAVECONF_SIZE
#define SAVECONF_SIZE 0
#endif

#ifndef SAVEFILE_NAME
#define SAVEFILE_NAME "save.sav"
#endif

#ifndef SAVECONF_DEFAULT_BLOBSIZE
#define SAVECONF_DEFAULT_BLOBSIZE 32
#endif

///////////////
// display defines
///////////////

#ifndef DISPLAY_MODE
#define DISPLAY_MODE DISPLAY_MODE_RGB565
#endif

#ifndef DISPLAY_DEFAULT_BACKGROUND_COLOR
#define DISPLAY_DEFAULT_BACKGROUND_COLOR Color::black
#endif

#ifndef DISPLAY_DEFAULT_COLOR
#define DISPLAY_DEFAULT_COLOR Color::white
#endif

///////////////
// recording defines
///////////////

#ifndef MAX_IMAGE_RECORDING
#define MAX_IMAGE_RECORDING 5
#endif

///////////////
// sound defines
///////////////

#ifndef SOUND_CHANNELS
#define SOUND_CHANNELS 4
#endif
