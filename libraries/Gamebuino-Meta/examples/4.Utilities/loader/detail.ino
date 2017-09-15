
Image titleScreenImage;

bool titleScreenImageExists;
bool displayName;
void loadDetailedView() {
	strcpy(nameBuffer, getCurrentGameFolder());
	strcpy(nameBuffer + strlen(nameBuffer), "/TITLESCREEN.BMP");
	titleScreenImageExists = SD.exists(nameBuffer);
	displayName = !titleScreenImageExists;
	if (!titleScreenImageExists) {
		// try to display the first BMP in the rec folder
		strcpy(nameBuffer, getCurrentGameFolder());
		strcpy(nameBuffer + strlen(nameBuffer), "/REC/");
		uint16_t i = strlen(nameBuffer);
		if (SD.exists(nameBuffer)) {
			File dir_walk = SD.open(nameBuffer);
			File entry;
			while (entry = dir_walk.openNextFile()) {
				if (!entry.isFile()) {
					continue;
				}
				entry.getName(nameBuffer + i, NAMEBUFFER_LENGTH - i);
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
		gb.language.println(lang_loading);
		gb.updateDisplay();
		gb.display.init(80, 64, nameBuffer);
		if (gb.display.width() != 80 || gb.display.height() != 64) {
			titleScreenImageExists = false;
		}
	}
	if (!titleScreenImageExists) {
		gb.display.init(80, 64, ColorMode::rgb565);
	}
}

void loadGame() {
	strcpy(folderName, getCurrentGameFolder());
	getBinPath(nameBuffer);
	if (titleScreenImageExists) {
		loadDetailedView(); // easiest way to fetch the first frame
	} else {
		gb.display.clear();
		gb.display.setColor(WHITE, BLACK);
		gb.display.setCursors(0, 24);
		gb.language.println(lang_loading);
		gb.display.print(nameBuffer);
	}
	gb.updateDisplay();
	gb.bootloader.game(nameBuffer);
}

void detailedView() {
	loadDetailedView();
	while (1) {
		if (!gb.update()) {
			continue;
		}
		uint8_t blockOffset = currentGame / BLOCK_LENGTH;
		uint8_t gameInBlock = currentGame % BLOCK_LENGTH;
		uint8_t b = getBlock(blockOffset);
		
		if (titleScreenImageExists) {
			gb.display.nextFrame();
		} else {
			gb.display.clear();
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
			gb.display.println(getCurrentGameFolder() + 1);
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
		gb.language.print(lang_select);
		
		// < > BROWSE
		gb.display.setCursorX(43);
		gb.display.setColor(LIGHTBLUE);
		gb.display.print("<");
		gb.display.setCursorX(49);
		gb.display.print(">");
		gb.display.setCursorX(55);
		gb.display.setColor(BROWN);
		gb.language.print(lang_browse);
		
		if (gb.buttons.repeat(BUTTON_LEFT, 4) && currentGame > 0) {
			currentGame--;
			loadDetailedView();
			gb.sound.playTick();
		}
		
		if (gb.buttons.repeat(BUTTON_RIGHT, 4) && currentGame < totalGames - 1) {
			currentGame++;
			loadDetailedView();
			gb.sound.playTick();
		}
		
		if (gb.buttons.pressed(BUTTON_A)) {
			loadGame();
		}
		
		if (gb.buttons.pressed(BUTTON_B)) {
			gb.display.init(80, 64, ColorMode::rgb565);
			gb.sound.playCancel();
			return;
		}
	}
}
