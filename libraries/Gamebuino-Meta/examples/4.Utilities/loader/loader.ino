#include <Gamebuino-Meta.h>

#define MAX_FOLDER_NAME_LENGTH 40
#define BLOCK_LENGTH 64
char gameFolders[BLOCK_LENGTH][MAX_FOLDER_NAME_LENGTH];
uint8_t filesInBlock;
bool lastBlock = false;
char folderName[512];
char nameBuffer[512];
uint32_t currentGame = 0;
uint32_t gameFolderBlock = 0;

void clearEmptyFolders() {
	File dir_walk = SD.open("/");
	File entry;
	while (entry = dir_walk.openNextFile()) {
		if (!entry.isDirectory()) {
			continue;
		}
		folderName[0] = '/';
		entry.getName(folderName+1, 512-1);
		if (!strstr(folderName, "loader")) {
			SD.rmdir(folderName); // this already checks if the folder is empty
		}
	}
	entry.close();
	dir_walk.close();
}

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
		entry.getName(name + i, 512 - i);
		if (strstr(name, ".bin") || strstr(name, ".BIN")) {
			return true;
		}
	}
	return false;
}

void loadGameFolderBlock() {
	File root = SD.open("/");
	File entry;
	filesInBlock = 0;
	uint32_t i = 0;
	uint32_t j = 0;
	bool searchForLast = false;
	lastBlock = true;
	while (entry = root.openNextFile()) {
		if (!entry.isDirectory()) {
			continue;
		}
		gameFolders[filesInBlock][0] = '/';
		entry.getName(gameFolders[filesInBlock] + 1, MAX_FOLDER_NAME_LENGTH - 1);
		strcpy(folderName, gameFolders[filesInBlock]);
		
		if (!getBinPath(nameBuffer)) {
			continue;
		}
		if (searchForLast) {
			lastBlock = false;
			break;
		}
		if (i < gameFolderBlock) {
			if (j < BLOCK_LENGTH) {
				j++;
				continue;
			}
			i++;
			j = 0;
			if (i < gameFolderBlock) {
				continue;
			}
		}
		filesInBlock++;
		if (filesInBlock >= BLOCK_LENGTH) {
			searchForLast = true;
		}
	}
}


Image titleScreenImage;
bool titleScreenImageExists;
void loadDetailedView() {
	uint16_t currentGameInBlock = currentGame % BLOCK_LENGTH;
	strcpy(nameBuffer, gameFolders[currentGameInBlock]);
	strcpy(nameBuffer + strlen(nameBuffer), "/TITLESCREEN.BMP");
	titleScreenImageExists = SD.exists(nameBuffer);
	if (titleScreenImageExists) {
		titleScreenImage.init(80, 64, nameBuffer);
	}
}

void loadGame() {
	uint16_t currentGameInBlock = currentGame % BLOCK_LENGTH;
	strcpy(folderName, gameFolders[currentGameInBlock]);
	getBinPath(nameBuffer);
	if (titleScreenImageExists) {
		gb.display.drawImage(0, 0, titleScreenImage);
	} else {
		gb.display.fillScreen(BLACK);
		gb.display.setColor(WHITE, BLACK);
		gb.display.setCursors(0, 24);
		gb.display.println("Loading...");
		gb.display.print(nameBuffer);
	}
	gb.tft.drawImage(0, 0, gb.display, gb.tft.width(), gb.tft.height());
	Gamebuino_Meta::load_game(nameBuffer);
}

void detailedView() {
	loadDetailedView();
	while (1) {
		if (!gb.update()) {
			continue;
		}
		uint16_t currentGameInBlock = currentGame % BLOCK_LENGTH;
		if (titleScreenImageExists) {
			gb.display.drawImage(0, 0, titleScreenImage);
		} else {
			gb.display.fillScreen(BLACK);
		}
		
		// center bar
		gb.display.setColor(ORANGE);
		gb.display.fillRect(0, 15, 80, 9);
		gb.display.setColor(BROWN);
		gb.display.drawFastHLine(0, 14, 80);
		gb.display.drawFastHLine(0, 24, 80);
		
		// game name
		gb.display.setColor(WHITE);
		gb.display.setCursors(2, 17);
		gb.display.println(gameFolders[currentGameInBlock] + 1);
		
		// lower bar
		gb.display.setColor(BROWN);
		gb.display.fillRect(0, 57, 80, 7);
		
		// A SELECT
		gb.display.setColor(GREEN);
		gb.display.setCursors(2, 58);
		gb.display.print("A");
		gb.display.setCursorX(8);
		gb.display.setColor(ORANGE);
		gb.display.print("SELECT");
		
		// < > BROWSE
		gb.display.setCursorX(43);
		gb.display.setColor(LIGHTBLUE);
		gb.display.print("<");
		gb.display.setCursorX(49);
		gb.display.print(">");
		gb.display.setCursorX(55);
		gb.display.setColor(ORANGE);
		gb.display.print("BROWSE");
		
		if (gb.buttons.pressed(BUTTON_LEFT)) {
			if (currentGameInBlock == 0 && gameFolderBlock == 0) {
				// nothing to do
			} else if (currentGameInBlock == 0) {
				currentGame--;
				gameFolderBlock--;
				loadGameFolderBlock();
				loadDetailedView();
			} else {
				currentGame--;
				loadDetailedView();
			}
		}
		
		if (gb.buttons.pressed(BUTTON_RIGHT)) {
			if (currentGameInBlock >= (filesInBlock - 1) && lastBlock) {
				// do nothing
			} else if (currentGameInBlock >= (filesInBlock - 1)) {
				currentGame++;
				gameFolderBlock++;
				loadGameFolderBlock();
				loadDetailedView();
			} else {
				currentGame++;
				loadDetailedView();
			}
		}
		
		if (gb.buttons.pressed(BUTTON_A)) {
			loadGame();
		}
	}
}

void setup() {
	gb.begin();
	clearEmptyFolders();
	loadGameFolderBlock();
	
	detailedView();
}

void loop() {
	
}
