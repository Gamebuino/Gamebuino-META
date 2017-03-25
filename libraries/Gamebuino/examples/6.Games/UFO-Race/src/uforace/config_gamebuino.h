#define GAMEBUINO_TEST "game"


#define SAVECONF_SIZE 2
#define SAVECONF { \
		SaveDefault(0, SAVETYPE_INT, 4), \
		SaveDefault(15, SAVETYPE_BLOB, "THE GAME", 10), \
	}
