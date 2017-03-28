#ifndef FOLDER_NAME
#ifdef __SKETCH_NAME__
#define FOLDER_NAME __SKETCH_NAME__
#else
#define FOLDER_NAME "testsketch"
#endif
#endif



// save defines
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
