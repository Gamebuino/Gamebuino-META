
const uint8_t starBuf[] = {
	15, 15,
	0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x77, 0xA0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x7A, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x07, 0xAF, 0x77, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0A, 0xF7, 0x7A, 0x00, 0x00, 0x00,
	0x7A, 0xA7, 0xAF, 0x77, 0xAA, 0x7F, 0x99, 0xA0,
	0x0A, 0x7A, 0xF7, 0x7A, 0xA7, 0xF9, 0x9A, 0x00,
	0x00, 0xAF, 0x77, 0xAA, 0x7F, 0x99, 0xA0, 0x00,
	0x00, 0x07, 0x7A, 0xA7, 0xF9, 0x9A, 0x00, 0x00,
	0x00, 0x00, 0xAA, 0x7F, 0x99, 0xA0, 0x00, 0x00,
	0x00, 0x00, 0xA7, 0xF9, 0x9A, 0xF0, 0x00, 0x00,
	0x00, 0x0A, 0x7F, 0x90, 0xAF, 0x99, 0x00, 0x00,
	0x00, 0x07, 0xF9, 0x00, 0x09, 0x9F, 0x00, 0x00,
	0x00, 0x7F, 0x90, 0x00, 0x00, 0xFA, 0x90, 0x00,
	0x00, 0xF9, 0x00, 0x00, 0x00, 0x09, 0xA0, 0x00,
};
Image star{starBuf};


bool titleScreenImageExists;
bool displayName;
bool detailGameIsFav;

const char* msg_a_start;
uint8_t msg_a_w;
uint8_t msg_a_h;
uint8_t msg_a_x;
uint8_t msg_a_y;
void loadDetailedView() {
	star.setTransparentColor(INDEX_BLACK);
	detailGameIsFav = isGameFavorite();
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
				if (!strstr(nameBuffer, ".GMV") && !strstr(nameBuffer, ".gmv")) {
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
		gb.display.setCursor(0, 0);
		gb.display.println(lang_loading);
		gb.updateDisplay();
		gb.display.init(nameBuffer);
		if ((gb.display.width() == 80 && gb.display.height() == 64) || (gb.display.width() == 160 && gb.display.height() == 128)) {
			gb.display.fontSize = gb.display.width() == 80 ? 1 : 2;
		} else {
			titleScreenImageExists = false;
		}
	}
	if (!titleScreenImageExists) {
		gb.display.init(80, 64, ColorMode::rgb565);
		gb.display.fontSize = 1;
	}
	
	msg_a_start = gb.language.get(lang_a_start);
	msg_a_w = gb.display.fontWidth*strlen(msg_a_start)*gb.display.fontSize;
	msg_a_h = gb.display.fontHeight*gb.display.fontSize;
	msg_a_x = (gb.display.width() - msg_a_w) / 2;
	msg_a_y = (gb.display.height() / 5) * 3 + msg_a_h;
}

void loadGame() {
	if (titleScreenImageExists) {
		loadDetailedView(); // easiest way to fetch the first frame
	}
	strcpy(folderName, getCurrentGameFolder());
	getBinPath(nameBuffer);
	if (!titleScreenImageExists) {
		gb.display.clear();
		gb.display.setColor(WHITE, BLACK);
		gb.display.setCursor(0, 24);
		gb.display.println(lang_loading);
		gb.display.print(nameBuffer);
	}
	gb.updateDisplay();
	gb.bootloader.game(nameBuffer);
}

void detailedView() {
	loadDetailedView();
	bool first = true;
	bool reInitDisplay = false;
	while (1) {
		if (!gb.update()) {
			continue;
		}
		gb.display.fontSize = gb.display.width() == 80 ? 1 : 2;
		if (reInitDisplay && titleScreenImageExists) {
			if (gb.display.frames == 1) {
				gb.display.init(nameBuffer);
			}
			reInitDisplay = false;
		}
		uint8_t blockOffset = currentGame / BLOCK_LENGTH;
		uint8_t gameInBlock = currentGame % BLOCK_LENGTH;
		uint8_t b = getBlock(blockOffset);
		
		if (titleScreenImageExists) {
			gb.display.nextFrame();
		} else {
			gb.display.clear();
		}
		
		if (detailGameIsFav) {
			gb.display.drawImage(0, 0, star, star.width()*gb.display.fontSize, star.height()*gb.display.fontSize);
		}
		
		if (displayName) {
			// center bar
			gb.display.setColor(BROWN);
			gb.display.fillRect(0, 15*gb.display.fontSize, 80*gb.display.fontSize, 9*gb.display.fontSize);
			gb.display.setColor(DARKGRAY);
			gb.display.drawFastHLine(0, 14*gb.display.fontSize, 80*gb.display.fontSize);
			gb.display.drawFastHLine(0, 24*gb.display.fontSize, 80*gb.display.fontSize);
			if (gb.display.fontSize > 1) {
				gb.display.drawFastHLine(0, 14*gb.display.fontSize + 1, 80*gb.display.fontSize);
				gb.display.drawFastHLine(0, 24*gb.display.fontSize + 1, 80*gb.display.fontSize);
			}
			
			// game name
			gb.display.setColor(WHITE);
			gb.display.setCursor(2*gb.display.fontSize, 17*gb.display.fontSize);
			gb.display.println(getCurrentGameFolder() + 1);
		}
		
		// flashing "A to start"
		if ((gb.frameCount % 32) < 20) {
			gb.display.setColor(DARKGRAY);
			gb.display.drawRect(msg_a_x - gb.display.fontSize*2, msg_a_y - gb.display.fontSize*2, msg_a_w + gb.display.fontSize*4, msg_a_h + gb.display.fontSize*3);
			if (gb.display.fontSize > 1) {
				gb.display.drawRect(msg_a_x - gb.display.fontSize*2 + 1, msg_a_y - gb.display.fontSize*2 + 1, msg_a_w + gb.display.fontSize*4 - 2, msg_a_h + gb.display.fontSize*3 - 2);
			}
			gb.display.setColor(BROWN);
			gb.display.fillRect(msg_a_x - gb.display.fontSize, msg_a_y - gb.display.fontSize, msg_a_w + gb.display.fontSize*2, msg_a_h + gb.display.fontSize);
			gb.display.setColor(WHITE);
			gb.display.setCursor(msg_a_x, msg_a_y);
			gb.display.print(msg_a_start);
			first = true;
		} else if (titleScreenImageExists && gb.display.frames == 1 && first) {
			reInitDisplay = true;
			first = false;
		}
		
		if (gb.buttons.repeat(BUTTON_LEFT, 4)) {
			if (currentGame > 0) {
				currentGame--;
			} else {
				currentGame = totalGames - 1;
			}
			loadDetailedView();
		}
		
		if (gb.buttons.repeat(BUTTON_RIGHT, 4)) {
			if (currentGame < totalGames - 1) {
				currentGame++;
			} else {
				currentGame = 0;
			}
			loadDetailedView();
		}
		
		if (gb.buttons.pressed(BUTTON_A)) {
			loadGame();
		}
		
		if (gb.buttons.pressed(BUTTON_B)) {
			gb.display.init(80, 64, ColorMode::rgb565);
			return;
		}
		
		if (gb.buttons.pressed(BUTTON_DOWN)) {
			galleryView(1);
			loadDetailedView();
			continue;
		}
		
		if (gb.buttons.pressed(BUTTON_UP)) {
			galleryView(-1);
			loadDetailedView();
			continue;
		}
		
		if (gb.buttons.pressed(BUTTON_C)) {
			if (detailGameIsFav) {
				unfavoriteGame();
				gb.popup(gb.language.get(lang_fav_removed), 50);
			} else {
				favoriteGame();
				gb.popup(gb.language.get(lang_fav_added), 50);
			}
			loadDetailedView();
			continue;
		}
	}
}
