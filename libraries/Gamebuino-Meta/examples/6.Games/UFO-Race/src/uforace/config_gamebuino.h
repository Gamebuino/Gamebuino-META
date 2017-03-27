#define GAMEBUINO_TEST "game"

#define SAVEBLOCK_NUM 128
#define SAVECONF_SIZE 3
#define SAVECONF { \
		SaveDefault(0, SAVETYPE_INT, 4), \
		SaveDefault(2, SAVETYPE_BLOB, "foxfoxfox", 15), \
		SaveDefault(15, SAVETYPE_BLOB, "THE GAME", 10), \
	}
