
Image titleScreenImage;
bool titleScreenImageExists;
bool displayName;
void loadDetailedView() {
	uint16_t currentGameInBlock = currentGame % BLOCK_LENGTH;
	strcpy(nameBuffer, gameFolders[currentGameInBlock]);
	strcpy(nameBuffer + strlen(nameBuffer), "/TITLESCREEN.BMP");
	titleScreenImageExists = SD.exists(nameBuffer);
	displayName = !titleScreenImageExists;
	if (!titleScreenImageExists) {
		// try to display the first BMP in the rec folder
		strcpy(nameBuffer, gameFolders[currentGameInBlock]);
		strcpy(nameBuffer + strlen(nameBuffer), "/REC/");
		uint16_t i = strlen(nameBuffer);
		if (SD.exists(nameBuffer)) {
			File dir_walk = SD.open(nameBuffer);
			File entry;
			while (entry = dir_walk.openNextFile()) {
				if (!entry.isFile()) {
					continue;
				}
				entry.getName(nameBuffer + i, 512 - i);
				if (!strstr(nameBuffer, ".BMP") && !strstr(nameBuffer, ".bmp")) {
					continue;
				}
				titleScreenImageExists = true;
				displayName = true;
				break;
			}
		}
	}
	if (titleScreenImageExists) {
		gb.display.setColor(WHITE, BLACK);
		gb.display.setCursors(0, 0);
		gb.display.println(gb.language.get(lang_loading));
		gb.updateDisplay();
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
		gb.display.println(gb.language.get(lang_loading));
		gb.display.print(nameBuffer);
	}
	gb.updateDisplay();
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
		
		if (displayName) {
			// center bar
			gb.display.setColor(BROWN);
			gb.display.fillRect(0, 15, 80, 9);
			gb.display.setColor(DARKGRAY);
			gb.display.drawFastHLine(0, 14, 80);
			gb.display.drawFastHLine(0, 24, 80);
			
			// game name
			gb.display.setColor(WHITE);
			gb.display.setCursors(2, 17);
			gb.display.println(gameFolders[currentGameInBlock] + 1);
		}
		
		// lower bar
		gb.display.setColor(DARKGRAY);
		gb.display.fillRect(0, 57, 80, 7);
		
		// A SELECT
		gb.display.setColor(GREEN);
		gb.display.setCursors(2, 58);
		gb.display.print("A");
		gb.display.setCursorX(8);
		gb.display.setColor(BROWN);
		gb.display.print(gb.language.get(lang_select));
		
		// < > BROWSE
		gb.display.setCursorX(43);
		gb.display.setColor(LIGHTBLUE);
		gb.display.print("<");
		gb.display.setCursorX(49);
		gb.display.print(">");
		gb.display.setCursorX(55);
		gb.display.setColor(BROWN);
		gb.display.print(gb.language.get(lang_browse));
		
		if (gb.buttons.repeat(BUTTON_LEFT, 4)) {
			if (currentGameInBlock == 0 && gameFolderBlock == 0) {
				// nothing to do
			} else if (currentGameInBlock == 0) {
				currentGame--;
				gameFolderBlock--;
				loadGameFolderBlock();
				loadDetailedView();
				gb.sound.playTick();
			} else {
				currentGame--;
				loadDetailedView();
				gb.sound.playTick();
			}
		}
		
		if (gb.buttons.repeat(BUTTON_RIGHT, 4)) {
			if (currentGameInBlock >= (filesInBlock - 1) && lastBlock) {
				// do nothing
			} else if (currentGameInBlock >= (filesInBlock - 1)) {
				currentGame++;
				gameFolderBlock++;
				loadGameFolderBlock();
				loadDetailedView();
				gb.sound.playTick();
			} else {
				currentGame++;
				loadDetailedView();
				gb.sound.playTick();
			}
		}
		
		if (gb.buttons.pressed(BUTTON_A)) {
			loadGame();
		}
		
		if (gb.buttons.pressed(BUTTON_C)) {
			gb.sound.playCancel();
			return;
		}
	}
}
