#include <Gamebuino-Meta.h>
#include "language.h"

const char LOADER_VERSION[] = "0.0.3";

#define RAM_FLAG_ADDRESS (0x20007FFCul)
#define RAM_FLAG_VALUE (*((volatile uint32_t *)RAM_FLAG_ADDRESS))
#define LOADER_MAGIC 0x242A0000

const uint32_t MAX_FOLDER_NAME_LENGTH = 40;
const uint8_t NAMEBUFFER_LENGTH = (MAX_FOLDER_NAME_LENGTH*2);
const uint8_t PAGE_SIZE = 6;
const uint8_t PAGES_PER_BLOCK = 2;
const uint8_t BLOCK_LENGTH = (PAGE_SIZE * PAGES_PER_BLOCK);
uint8_t blocksLoaded[2];
uint32_t totalGames = 0;
char folderName[MAX_FOLDER_NAME_LENGTH];
char nameBuffer[NAMEBUFFER_LENGTH];
char gameFolders[2][BLOCK_LENGTH][MAX_FOLDER_NAME_LENGTH];
uint32_t currentGame = 0;

const uint8_t MAX_FAV_GAMES = 10;
const uint8_t SAVE_NUM_FAVS = 0;
const uint8_t SAVE_FAVOFFSET = 1;
const SaveDefault savefileDefaults[] = {
	{ SAVE_NUM_FAVS, SAVETYPE_INT, 0, 0 },
	{ 1, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
	{ 2, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
	{ 3, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
	{ 4, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
	{ 5, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
	{ 6, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
	{ 7, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
	{ 8, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
	{ 9, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
	{10, SAVETYPE_BLOB, MAX_FOLDER_NAME_LENGTH, 0 },
};

const uint8_t GAMEBUINO_LOGO[] = {80,10,
	0b00111100,0b00111111,0b00111111,0b11110011,0b11110011,0b11110011,0b00110011,0b00111111,0b00111111,0b00011100,
	0b00111100,0b00111111,0b00111111,0b11110011,0b11110011,0b11110011,0b00110011,0b00111111,0b00111111,0b00100110,
	0b00110000,0b00110011,0b00110011,0b00110011,0b00000011,0b00110011,0b00110011,0b00110011,0b00110011,0b00100110,
	0b00110000,0b00110011,0b00110011,0b00110011,0b00000011,0b00110011,0b00110011,0b00110011,0b00110011,0b00101010,
	0b00110011,0b00111111,0b00110011,0b00110011,0b11110011,0b11000011,0b00110011,0b00110011,0b00110011,0b00011100,
	0b00110011,0b00111111,0b00110011,0b00110011,0b11110011,0b11000011,0b00110011,0b00110011,0b00110011,0b00000000,
	0b00110011,0b00110011,0b00110011,0b00110011,0b00000011,0b00110011,0b00110011,0b00110011,0b00110011,0b00000000,
	0b00110011,0b00110011,0b00110011,0b00110011,0b00000011,0b00110011,0b00110011,0b00110011,0b00110011,0b00000000,
	0b00111111,0b00110011,0b00110011,0b00110011,0b11110011,0b11110011,0b11110011,0b00110011,0b00111111,0b00000000,
	0b00111111,0b00110011,0b00110011,0b00110011,0b11110011,0b11110011,0b11110011,0b00110011,0b00111111,0b00000000,
};

const char GAMEFOLDERS_CACHE_FILE[] = "GAMEFOLDERS.CACHE";

bool getBinPath(char* name) {
	File dir_walk = SD.open(folderName);
	File entry;
	strcpy(name, folderName);
	strcpy(name + strlen(folderName), folderName);
	strcpy(name + strlen(name), ".bin");
	if (SD.exists(name)) {
		return true;
	}
	strcpy(name, folderName);
	strcpy(name + strlen(name), "/");
	uint16_t i = strlen(name);
	while (entry = dir_walk.openNextFile()) {
		if (!entry.isFile()) {
			continue;
		}
		entry.getName(name + i, NAMEBUFFER_LENGTH - i);
		if (strstr(name, ".bin") || strstr(name, ".BIN")) {
			return true;
		}
	}
	return false;
}

void initFolders() {
	File root = SD.open("/");
	File entry;
	totalGames = 0;
	while (entry = root.openNextFile()) {
		if (!entry.isDirectory()) {
			continue;
		}
		folderName[0] = '/';
		entry.getName(folderName + 1, MAX_FOLDER_NAME_LENGTH - 1);
		if (!strstr(folderName, "loader")) {
			SD.rmdir(folderName); // this already checks if the folder is empty
		}
		if (!getBinPath(nameBuffer)) {
			continue;
		}
		totalGames++;
	}
	entry.close();
	root.close();
}

void createCache() {
	File cache;
	if (SD.exists(GAMEFOLDERS_CACHE_FILE)) {
		cache = SD.open(GAMEFOLDERS_CACHE_FILE, FILE_WRITE);
		cache.rewind();
		uint32_t num;
		cache.read(&num, 4);
		uint32_t games;
		cache.read(&games, 4);
		cache.close();
		if (games == totalGames && num == MAX_FOLDER_NAME_LENGTH) {
			uint8_t numBlocks = (totalGames + BLOCK_LENGTH - 1) / BLOCK_LENGTH;
			bool invalid = false;
			for (uint8_t b = 0; b < numBlocks; b++) {
				loadGameFolderBlock(0, b);
				for (uint8_t i = 0; i < BLOCK_LENGTH; i++) {
					if (b*BLOCK_LENGTH + i < totalGames && !SD.exists(gameFolders[0][i])) {
						invalid = true;
						break;
					}
				}
				if (invalid) {
					break;
				}
			}
			if (!invalid) {
				loadGameFolderBlocks();
				return; //everything is OK
			}
		}
	}
	
	SD.remove(GAMEFOLDERS_CACHE_FILE);
	cache = SD.open(GAMEFOLDERS_CACHE_FILE, FILE_WRITE);
	cache.rewind();
	cache.write(&MAX_FOLDER_NAME_LENGTH, 4);
	cache.write(&totalGames, 4);
	uint8_t filesInBlock = 0;
	uint8_t numBlocks = 0;
	
	// first we load the favourited games
	char* favGames = (char*)gb.display._buffer; // this should be large enough
	memset(favGames, 0, MAX_FOLDER_NAME_LENGTH*SAVE_NUM_FAVS);
	uint8_t maxFavs = gb.save.get(SAVE_NUM_FAVS);
	if (maxFavs) {
		for (uint8_t i = 0; i < maxFavs; i++) {
			gb.save.get(SAVE_FAVOFFSET + i, folderName);
			if (!SD.exists(folderName)) {
				currentGame = i;
				unfavoriteGame(); // this will re-build cache again, so we recursively fix our list
				return;
			}
			memcpy(favGames + (i*MAX_FOLDER_NAME_LENGTH), folderName, MAX_FOLDER_NAME_LENGTH);
			memcpy(gameFolders[0][filesInBlock], folderName, MAX_FOLDER_NAME_LENGTH);
			filesInBlock++;
			if (filesInBlock >= BLOCK_LENGTH) {
				cache.write(gameFolders[0], BLOCK_LENGTH*MAX_FOLDER_NAME_LENGTH);
				numBlocks++;
				filesInBlock = 0;
			}
		}
	}
	
	File root = SD.open("/");
	File entry;
	while (entry = root.openNextFile()) {
		if (!entry.isDirectory()) {
			continue;
		}
		folderName[0] = '/';
		entry.getName(folderName + 1, MAX_FOLDER_NAME_LENGTH - 1);
		if (!getBinPath(nameBuffer)) {
			continue;
		}
		bool isFav = false;
		for (uint8_t i = 0; i < maxFavs; i++) {
			if (strcmp(folderName, favGames + (i*MAX_FOLDER_NAME_LENGTH)) == 0) {
				isFav = true;
				break;
			}
		}
		if (isFav) {
			continue;
		}
		strncpy(gameFolders[0][filesInBlock], folderName, MAX_FOLDER_NAME_LENGTH);
		filesInBlock++;
		if (filesInBlock >= BLOCK_LENGTH) {
			cache.write(gameFolders[0], BLOCK_LENGTH*MAX_FOLDER_NAME_LENGTH);
			numBlocks++;
			filesInBlock = 0;
		}
	}
	if (filesInBlock < BLOCK_LENGTH) {
		// still gotta cache the last block
		cache.write(gameFolders[0], filesInBlock*MAX_FOLDER_NAME_LENGTH);
	}
	cache.close();
	entry.close();
	root.close();
	
	loadGameFolderBlocks();
}

uint8_t getBlock(uint8_t b) {
	if (blocksLoaded[0] == b) {
		return 0;
	}
	if (blocksLoaded[1] == b) {
		return 1;
	}
	int8_t diff = blocksLoaded[0] - b;
	if (diff <= 1 && diff >= -1) {
		loadGameFolderBlock(1, b);
		blocksLoaded[1] = b;
		return 1;
	} else {
		loadGameFolderBlock(0, b);
		blocksLoaded[0] = b;
		return 0;
	}
}

char* getCurrentGameFolder() {
	uint8_t blockOffset = currentGame / BLOCK_LENGTH;
	uint8_t gameInBlock = currentGame % BLOCK_LENGTH;
	uint8_t b = getBlock(blockOffset);
	return gameFolders[b][gameInBlock];
}

void unfavoriteGame() {
	char* identifier = getCurrentGameFolder();
	uint8_t maxFavs = gb.save.get(SAVE_NUM_FAVS);
	bool moving = false;
	for (uint8_t i = 0; i < maxFavs; i++) {
		gb.save.get(SAVE_FAVOFFSET + i, nameBuffer);
		if (strcmp(nameBuffer, identifier) == 0) {
			moving = true;
		} else if (moving) {
			gb.save.set(SAVE_FAVOFFSET + i - i, nameBuffer);
		}
	}
	gb.save.del(SAVE_FAVOFFSET + maxFavs - 1);
	gb.save.set(SAVE_NUM_FAVS, maxFavs - 1);
	
	char backup[MAX_FOLDER_NAME_LENGTH];
	strcpy(backup, identifier); // let's save it...
	
	SD.remove(GAMEFOLDERS_CACHE_FILE);
	createCache();
	
	currentGame = maxFavs - 1;
	while (strcmp(backup, getCurrentGameFolder()) != 0) {
		currentGame++;
	}
}

bool favoriteGame() {
	if (isGameFavorite()) {
		return true;
	}
	uint8_t maxFavs = gb.save.get(SAVE_NUM_FAVS);
	if (maxFavs >= MAX_FAV_GAMES) {
		return false;
	}
	gb.save.set(SAVE_FAVOFFSET + maxFavs, getCurrentGameFolder());
	gb.save.set(SAVE_NUM_FAVS, maxFavs + 1);
	
	SD.remove(GAMEFOLDERS_CACHE_FILE);
	createCache();
	currentGame = maxFavs; // no +1 because games start with zero
}

bool isGameFavorite() {
	char* identifier = getCurrentGameFolder();
	uint8_t maxFavs = gb.save.get(SAVE_NUM_FAVS);
	for (uint8_t i = 0; i < maxFavs; i++) {
		gb.save.get(SAVE_FAVOFFSET + i, nameBuffer);
		if (strcmp(nameBuffer, identifier) == 0) {
			return true;
		}
	}
	return false;
}

void loadGameFolderBlock(uint8_t b, uint8_t bb) {
	File cache = SD.open(GAMEFOLDERS_CACHE_FILE, FILE_WRITE);
	cache.seekSet(8 + bb*BLOCK_LENGTH*MAX_FOLDER_NAME_LENGTH);
	cache.read(gameFolders[b], BLOCK_LENGTH*MAX_FOLDER_NAME_LENGTH);
	cache.close();
}

void loadGameFolderBlocks() {
	loadGameFolderBlock(0, blocksLoaded[0]);
	loadGameFolderBlock(1, blocksLoaded[1]);
}

void setup() {
	gb.begin();
	gb.save.config(savefileDefaults);
//	SerialUSB.begin(115200);
//	while(!SerialUSB);
	if ((RAM_FLAG_VALUE & 0xFFFF0000) == LOADER_MAGIC) {
		uint16_t error = RAM_FLAG_VALUE & 0x0000FFFF;
		if (error && error != 0xD87A) {
			gb.display.clear();
			gb.display.print("ERROR (");
			gb.display.print(error);
			gb.display.println(")");
			switch (error) {
				case 0:
					gb.display.println("no error");
					break;
				case 1:
					gb.display.println("NMI fault");
					break;
				case 2:
					gb.display.println("hard fault");
					break;
				case 3:
					gb.display.println("WDT reset");
					break;
				case 4:
					gb.display.println("Invalid program");
					break;
				default:
					gb.display.println("Unknown");
			}
			gb.display.print("\n");
			gb.display.print(lang_press_a_continue);
			while (1) {
				while(!gb.update());
				if (gb.buttons.pressed(BUTTON_A)) {
					break;
				}
			}
		}
		RAM_FLAG_VALUE = 0; // make sure we don't get weird stuff
	}
	
	gb.display.clear();
	gb.display.drawBitmap(0, 2, GAMEBUINO_LOGO);
	gb.display.setCursor(0, 18);
	gb.display.println(lang_loading);
	gb.updateDisplay();
	initFolders();
	blocksLoaded[0] = 0;
	blocksLoaded[1] = 1;
	createCache();
	
	gridView();
}

void loop() {
	
}
