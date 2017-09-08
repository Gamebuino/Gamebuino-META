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

void loadGridView() {
	gb.display.setCursors(0, 0);
	gb.display.setColor(WHITE, BLACK);
	gb.display.println(gb.language.get(lang_loading));
	gb.updateDisplay();
	uint8_t blockOffset = pageInBlock*PAGE_SIZE;
	uint32_t gameOffset = gameFolderBlock*BLOCK_LENGTH + blockOffset;
	for (uint8_t i = 0; i < PAGE_SIZE; i++) {
		if (gameOffset + i >= totalGames) {
			gridViewEntries[i].mode = GridMode::none;
		} else {
			strcpy(nameBuffer, gameFolders[blockOffset + i]);
			strcpy(nameBuffer + strlen(nameBuffer), "/ICON.BMP");
			if (SD.exists(nameBuffer)) {
				gridViewEntries[i].mode = GridMode::icon;
				gridViewEntries[i].img.init(19, 22, nameBuffer);
			} else {
				gridViewEntries[i].mode = GridMode::name;
			}
		}
	}
}

void gridView() {
	loadGridView();
	char singleLine[5] = "XXXX";
	char pageCounter[20];
	uint8_t x = 0;
	uint8_t y = 0;
	uint16_t totalPages = (totalGames / PAGE_SIZE) + 1;
	if ((totalGames % PAGE_SIZE) == 0) {
		totalPages--;
	}
	while(1) {
		if (!gb.update()) {
			continue;
		}
		if (!totalGames) {
			gb.display.setColor(RED);
			gb.display.println(gb.language.get(lang_no_games));
		} else {
			uint16_t currentPage = pageInBlock + gameFolderBlock*PAGES_PER_BLOCK + 1;
			sprintf(pageCounter, "< %d/%d >", currentPage, totalPages);
			gb.display.setColor(WHITE);
			gb.display.setCursors(40 - strlen(pageCounter)*2, 1);
			gb.display.print(pageCounter);
			uint8_t gridCounter = 0;
			uint8_t blockOffset = pageInBlock*PAGE_SIZE;
			uint32_t gameOffset = gameFolderBlock*BLOCK_LENGTH + blockOffset;
			
			for (uint8_t j = 0; j < GRID_HEIGHT; j++) {
				for (uint8_t i = 0; i < GRID_WIDTH; i ++) {
					gb.display.setColor(DARKGRAY);
					gb.display.drawRect(i*19 + 1, j*22 + 8, 20, 23);
					if (gridViewEntries[gridCounter].mode == GridMode::icon) {
						gb.display.drawImage(i*19 + 2, j*22 + 9, gridViewEntries[gridCounter].img);
					} else if (gridViewEntries[gridCounter].mode == GridMode::name) {
						memcpy(singleLine, gameFolders[gridCounter + blockOffset] + 1, 4);
						gb.display.setColor(WHITE);
						gb.display.setCursors(i*19 + 3, j*22 + 13);
						gb.display.print(singleLine);
						memcpy(singleLine, gameFolders[gridCounter + blockOffset] + 5, 4);
						gb.display.setCursors(i*19 + 3, j*22 + 13 + 8);
						gb.display.print(singleLine);
					}
					gridCounter++;
				}
			}
			if (gb.buttons.repeat(BUTTON_RIGHT, 4)) {
				if (x >= (GRID_WIDTH - 1)) {
					if (currentPage == totalPages) {
						// do nothing
					} else if (pageInBlock >= PAGES_PER_BLOCK - 1) {
						gameFolderBlock++;
						pageInBlock = 0;
						loadGameFolderBlock();
						loadGridView();
						x = 0;
					} else {
						pageInBlock++;
						loadGridView();
						x = 0;
					}
					if (x == 0) {
						// we switched page
						if (filesInBlock <= (pageInBlock*PAGE_SIZE + (GRID_WIDTH)*y)) {
							y--;
						}
						gb.sound.playTick();
					}
				} else if (gameOffset + y*GRID_WIDTH + x + 1 < totalGames) {
					x++;
					gb.sound.playTick();
				}
			}
			if (gb.buttons.repeat(BUTTON_LEFT, 4)) {
				if (x == 0) {
					if (currentPage == 1) {
						// do nothing
					} else if (pageInBlock == 0) {
						gameFolderBlock--;
						pageInBlock = PAGES_PER_BLOCK - 1;
						loadGameFolderBlock();
						loadGridView();
						gb.sound.playTick();
						x = GRID_WIDTH - 1;
					} else {
						pageInBlock--;
						loadGridView();
						x = GRID_WIDTH - 1;
						gb.sound.playTick();
					}
				} else {
					x--;
					gb.sound.playTick();
				}
			}
			if (gb.buttons.repeat(BUTTON_UP, 4)) {
				if (y > 0) {
					y--;
					gb.sound.playTick();
				}
			}
			if (gb.buttons.repeat(BUTTON_DOWN, 4)) {
				if (y < (GRID_HEIGHT - 1) && gameOffset + y*GRID_WIDTH + x + GRID_WIDTH < totalGames) {
					y++;
					gb.sound.playTick();
				}
			}
			gb.display.setCursors(2, 55);
			gb.display.setColor(WHITE);
			gb.display.print(gameFolders[blockOffset + x + GRID_WIDTH*y] + 1);
			if (gb.buttons.pressed(BUTTON_A)) {
				gb.sound.playOK();
				currentGame = gameFolderBlock*BLOCK_LENGTH + pageInBlock*PAGE_SIZE + x + y*GRID_WIDTH;
				detailedView();
				pageInBlock = (currentGame % BLOCK_LENGTH) / PAGE_SIZE;
				loadGridView();
				x = (currentGame % PAGE_SIZE) % GRID_WIDTH;
				y = (currentGame % PAGE_SIZE) / GRID_WIDTH;
			}
			if ((gb.frameCount % 8) >= 4) {
				gb.display.setColor(BROWN);
				gb.display.drawRect(x*19 + 1, y*22 + 8, 20, 23);
			}
		}
	}
}
