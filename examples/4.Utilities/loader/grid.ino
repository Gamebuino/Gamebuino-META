enum class GridMode : uint8_t {
	none,
	name,
	icon,
};

struct GridView {
	Image img;
	GridMode mode;
};

GridView gridViewEntries[PAGE_SIZE];

const uint8_t ICON_WIDTH = 32;
const uint8_t ICON_HEIGHT = 32;

void loadGridEntry(uint8_t i, uint32_t game) {
	if (game >= totalGames) {
		gridViewEntries[i].mode = GridMode::none;
		return;
	}
	uint8_t blockOffset = game / BLOCK_LENGTH;
	uint8_t gameInBlock = game % BLOCK_LENGTH;
	
	uint8_t b = getBlock(blockOffset);
	strcpy(nameBuffer, gameFolders[b][gameInBlock]);
	strcpy(nameBuffer + strlen(nameBuffer), "/ICON.BMP");
	if (SD.exists(nameBuffer)) {
		gridViewEntries[i].mode = GridMode::icon;
		gridViewEntries[i].img.init(ICON_WIDTH, ICON_HEIGHT, nameBuffer);
		return;
	}
	// maybe we have an old, cached icon?
	// not used as apparently live cropping is fast enough
	/*
	strcpy(nameBuffer + strlen(gameFolders[b][gameInBlock]), "/ICON_CACHE.BMP");
	if (SD.exists(nameBuffer)) {
		gridViewEntries[i].mode = GridMode::icon;
		gridViewEntries[i].img.init(ICON_WIDTH, ICON_HEIGHT, nameBuffer);
		return;
	}
	*/
	// maybe we have a titlescreen to crop?
	strcpy(nameBuffer + strlen(gameFolders[b][gameInBlock]), "/TITLESCREEN.BMP");
	if (SD.exists(nameBuffer)) {
		gridViewEntries[i].img.init(ICON_WIDTH, ICON_HEIGHT, ColorMode::rgb565);
		gb.display.init(nameBuffer);
		bool valid = gb.display.width() == 80 && gb.display.height() == 64;
		if (valid) {
			gridViewEntries[i].img.drawImage((ICON_WIDTH - 80) / 2, (ICON_HEIGHT - 64) / 2, gb.display);
			gb.display.init(80, 64, ColorMode::rgb565);
			
			gridViewEntries[i].mode = GridMode::icon;
			return;
		} 
		gb.display.init(80, 64, ColorMode::rgb565);
	}
	
	// okay, maybe the user made a screenshot that we need to crop?
	strcpy(nameBuffer + strlen(gameFolders[b][gameInBlock]), "/REC/");
	if (SD.exists(nameBuffer)) {
		uint8_t j = strlen(nameBuffer);
		File dir_walk = SD.open(nameBuffer);
		File entry;
		while (entry = dir_walk.openNextFile()) {
			if (!entry.isFile()) {
				continue;
			}
			entry.getName(nameBuffer + j, NAMEBUFFER_LENGTH - j);
			if (!strstr(nameBuffer, ".GMV") && !strstr(nameBuffer, ".gmv")) {
				continue;
			}
			// OK, it's time to create a cropped image!
			gridViewEntries[i].img.init(ICON_WIDTH, ICON_HEIGHT, ColorMode::rgb565);
			gb.display.init(nameBuffer);
			bool valid = gb.display.width() == 80 && gb.display.height() == 64;
			if (valid) {
				gridViewEntries[i].img.drawImage((ICON_WIDTH - 80) / 2, (ICON_HEIGHT - 64) / 2, gb.display);
			} 
			gb.display.init(80, 64, ColorMode::rgb565);
			
			gridViewEntries[i].mode = valid ? GridMode::icon : GridMode::name;
			return;
		}
	}
	gridViewEntries[i].mode = GridMode::name;
}

void loadGridView() {
	uint32_t gameOffset;
	if (currentGame < 2) {
		gameOffset = 0;
	} else {
		gameOffset = ((currentGame / 2) - 1) * 2;
	}
	uint8_t blockOffset = gameOffset / BLOCK_LENGTH;
	for (uint8_t i = 0; i < PAGE_SIZE; i++) {
		loadGridEntry(i, gameOffset + i);
	}
}

#define GRID_WIDTH 4
#define GRID_HEIGHT 2

const uint8_t CAMERA_INITIAL = 15;

void gridView() {
	loadGridView();
	char singleLine[8];
	char pageCounter[20];
	uint16_t totalPages = (totalGames / PAGE_SIZE) + 1;
	uint8_t gridIndex = 0;
	if ((totalGames % PAGE_SIZE) == 0) {
		totalPages--;
	}
	int8_t cameraY = CAMERA_INITIAL;
	int8_t cameraY_actual = CAMERA_INITIAL;
	uint8_t cursorX = 0;
	uint8_t cursorY = 0;
	while(1) {
		if (!gb.update()) {
			continue;
		}
		gb.display.clear();
		if (!totalGames) {
			gb.display.setColor(WHITE);
			gb.display.drawBitmap(0, 2, GAMEBUINO_LOGO);
			gb.display.setColor(RED);
			gb.display.setCursor(0, 18);
			gb.display.println(lang_no_games);
			continue;
		}
		
		if (cameraY != cameraY_actual) {
			int8_t dif = cameraY - cameraY_actual;
			if (dif < 0) {
				dif = -dif;
			}
			uint8_t adjust = 2;
			if (dif > 2) {
				adjust += 2;
			}
			if (dif > 10) {
				adjust += 4;
			}
			if (dif > 20) {
				adjust += 6;
			}
			if (dif > 30) {
				adjust += 8;
			}
			if (dif > 40) {
				adjust = dif - 40;
			}
			if (adjust > dif) {
				adjust = dif;
			}
			if (cameraY_actual < cameraY) {
				cameraY_actual += adjust;
			} else {
				cameraY_actual -= adjust;
			}
		}
		if (currentGame <= 3) {
			gb.display.drawBitmap(0, cameraY_actual - CAMERA_INITIAL + 2, GAMEBUINO_LOGO);
		}
		
		uint8_t i = gridIndex;
		int16_t yy = cameraY_actual;
		gb.display.setColor(WHITE);
		
		uint8_t cg = currentGame - cursorX - 2*cursorY;
		for (uint8_t j = 0; j < PAGE_SIZE; j++) {
			uint8_t xx = j % 2 ? 32 + 6 + 1 : 0 + 6;
			if (gridViewEntries[i].mode == GridMode::icon) {
				gb.display.drawImage(xx, yy, gridViewEntries[i].img);
			} else if (gridViewEntries[i].mode == GridMode::name) {
				uint8_t blockOffset = cg / BLOCK_LENGTH;
				uint8_t gameInBlock = cg % BLOCK_LENGTH;
				uint8_t b = getBlock(blockOffset);
				gb.display.setCursor(xx + 1, yy + 1);
				const char* n = gameFolders[b][gameInBlock] + 1;
				int8_t len = strlen(n);
				if (len > 7*4) {
					len = 7*4; // we don't want to display more than four rows
				}
				while (len > 0) {
					strncpy(singleLine, n, 7);
					len -= 7;
					n += 7;
					gb.display.print(singleLine);
					gb.display.cursorY += 7;
					gb.display.cursorX = xx + 1;
				}
			}
			cg++;
			
			if (j % 2) {
				yy += 33;
			}
			
			i++;
			if (i >= PAGE_SIZE) {
				i = 0;
			}
		}
		
		if ((gb.frameCount % 8) >= 4) {
			gb.display.setColor(WHITE);
			gb.display.drawRect(cursorX*33 + 6 - 1, cursorY*33 + cameraY_actual - 1, 34, 34);
		}
		
		if (gb.buttons.repeat(BUTTON_LEFT, 4) || gb.buttons.repeat(BUTTON_RIGHT, 4)) {
			if (cursorX == 1) {
				cursorX = 0;
				currentGame--;
			} else if (currentGame < totalGames - 1) {
				cursorX = 1;
				currentGame++;
			}
		}
		
		if (gb.buttons.repeat(BUTTON_UP, 4) && totalGames > 2) {
			if (currentGame >= 2) {
				currentGame -= 2;
				cursorY--;
				if (currentGame <= 1) {
					cameraY = CAMERA_INITIAL;
				} else {
					cameraY += 33;
				}
				if (cursorY < 1 && currentGame > 1) {
					gb.update(); // we want to already draw the screen
					cameraY -= 33;
					cameraY_actual -= 33;
					cursorY = 1;
					uint32_t cg = (currentGame / 2) * 2;
					if (gridIndex > 0) {
						gridIndex -= 2;
					} else {
						gridIndex = PAGE_SIZE - 2;
					}
					loadGridEntry(gridIndex, cg - 2);
					loadGridEntry(gridIndex + 1, cg - 1);
				}
			} else {
				// loop to bottom
				gridIndex = 0;
				cursorY = 1;
				currentGame = (((totalGames + 1) / 2) * 2) - 2 + cursorX;
				if (totalGames % 2 && cursorX == 1) {
					currentGame -= 2;
				}
				loadGridView();
				cameraY = -16;
			}
		}
		
		if (gb.buttons.repeat(BUTTON_DOWN, 4) && totalGames > 2) {
			if (currentGame < totalGames - 2) {
				currentGame += 2;
				cursorY++;
				if (currentGame <= 3) {
					// adjust the first row thing
					cameraY = -16;
				} else {
					cameraY -= 33;
				}
				if (cursorY > 1 && currentGame < totalGames - 1) {
					gb.update(); // we want to already draw the screen
					cameraY += 33;
					cameraY_actual += 33;
					cursorY = 1;
					uint32_t cg = (currentGame / 2) * 2;
					loadGridEntry(gridIndex, cg + 2);
					loadGridEntry(gridIndex + 1, cg + 3);
					
					gridIndex += 2;
					if (gridIndex >= PAGE_SIZE) {
						gridIndex = 0;
					}
				}
			} else {
				// loop to top
				currentGame = cursorX;
				cursorY = 0;
				gridIndex = 0;
				loadGridView();
				cameraY = CAMERA_INITIAL;
			}
		}
		
		if (gb.buttons.pressed(BUTTON_A)) {
			detailedView();
			gridIndex = 0;
			
			gb.display.clear();
			gb.display.println(lang_loading);
			gb.updateDisplay();
			loadGridView();
			cursorX = currentGame%2 ? 1 : 0;
			if (currentGame < 2) {
				cursorY = 0;
				cameraY = cameraY_actual = CAMERA_INITIAL;
			} else {
				cursorY = 1;
				cameraY = cameraY_actual = -16;
			}
			continue; // else the next c-button-press will trigger
		}
		
		if (gb.buttons.pressed(BUTTON_C)) {
			settingsView();
		}
	}
}
