#include <Gamebuino-Meta.h>
#include "language.h"

const char LOADER_VERSION[] = "dev";

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


const uint16_t startupSound[] = {0x0005,0x3089,0x208,0x238,0x7849,0x1468,0x0000};

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
			uint8_t numBlocks = totalGames / BLOCK_LENGTH;
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
				return; //everything is OK
			}
		}
	}
	
	SD.remove(GAMEFOLDERS_CACHE_FILE);
	cache = SD.open(GAMEFOLDERS_CACHE_FILE, FILE_WRITE);
	cache.rewind();
	cache.write(&MAX_FOLDER_NAME_LENGTH, 4);
	cache.write(&totalGames, 4);
	
	File root = SD.open("/");
	File entry;
	uint8_t filesInBlock = 0;
	uint8_t numBlocks = 0;
	while (entry = root.openNextFile()) {
		if (!entry.isDirectory()) {
			continue;
		}
		folderName[0] = '/';
		entry.getName(folderName + 1, MAX_FOLDER_NAME_LENGTH - 1);
		if (!getBinPath(nameBuffer)) {
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

void loadGameFolderBlock(uint8_t b, uint8_t bb) {
	File cache = SD.open(GAMEFOLDERS_CACHE_FILE, FILE_WRITE);
	cache.seekSet(8 + bb*BLOCK_LENGTH*MAX_FOLDER_NAME_LENGTH);
	cache.read(gameFolders[b], BLOCK_LENGTH*MAX_FOLDER_NAME_LENGTH);
	cache.close();
}

void setup() {
	gb.begin();
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
			gb.language.print(lang_press_a_continue);
			while (1) {
				if (!gb.update()) {
					continue;
				}
				if (gb.buttons.pressed(BUTTON_A)) {
					break;
				}
			}
		}
		RAM_FLAG_VALUE = 0; // make sure we don't get weird stuff
	}
	
	gb.display.clear();
	gb.display.drawBitmap(0, 2, GAMEBUINO_LOGO);
	gb.display.setCursors(0, 18);
	gb.language.println(lang_loading);
	gb.updateDisplay();
	initFolders();
	createCache();
	loadGameFolderBlock(0, 0);
	loadGameFolderBlock(1, 1);
	blocksLoaded[0] = 0;
	blocksLoaded[1] = 1;
	
	gb.sound.play(startupSound);
	
	
	gridView();
}

void loop() {
	
}
