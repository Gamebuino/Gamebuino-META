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
	} else {
		gridViewEntries[i].mode = GridMode::name;
	}
}

void loadGridView() {
	gb.display.setCursors(0, 0);
	gb.display.setColor(WHITE, BLACK);
	gb.language.println(lang_loading);
	gb.updateDisplay();
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

void gridView() {
	loadGridView();
	char singleLine[5] = "XXXX";
	char pageCounter[20];
	uint16_t totalPages = (totalGames / PAGE_SIZE) + 1;
	uint8_t gridIndex = 0;
	if ((totalGames % PAGE_SIZE) == 0) {
		totalPages--;
	}
	int8_t cameraY = 1;
	int8_t cameraY_actual = 1;
	uint8_t cursorX = 0;
	uint8_t cursorY = 0;
	while(1) {
		if (!gb.update()) {
			continue;
		}
		gb.display.clear();
		if (!totalGames) {
			gb.display.setColor(RED);
			gb.language.println(lang_no_games);
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
		
		uint8_t i = gridIndex;
		int16_t yy = cameraY_actual;
		gb.display.setColor(RED);
		for (uint8_t j = 0; j < PAGE_SIZE; j++) {
			uint8_t xx = j % 2 ? 32 + 6 + 1 : 0 + 6;
			if (gridViewEntries[i].mode == GridMode::icon) {
				gb.display.drawImage(xx, yy, gridViewEntries[i].img);
			}
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
		
		if (gb.buttons.repeat(BUTTON_LEFT, 4) && cursorX == 1) {
			cursorX = 0;
			currentGame--;
			gb.sound.playTick();
		}
		
		if (gb.buttons.repeat(BUTTON_RIGHT, 4) && cursorX == 0 && currentGame < totalGames - 1) {
			cursorX = 1;
			currentGame++;
			gb.sound.playTick();
		}
		
		if (gb.buttons.repeat(BUTTON_UP, 4) && currentGame >= 2) {
			currentGame -= 2;
			cursorY--;
			if (currentGame <= 1) {
				cameraY = 1;
			} else {
				cameraY += 33;
			}
			if (cursorY < 1 && currentGame > 1) {
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
			gb.sound.playTick();
		}
		
		if (gb.buttons.repeat(BUTTON_DOWN, 4) && currentGame < totalGames - 2) {
			currentGame += 2;
			cursorY++;
			if (currentGame <= 3) {
				// adjust the first row thing
				cameraY = -16;
			} else {
				cameraY -= 33;
			}
			if (cursorY > 1 && currentGame < totalGames - 1) {
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
			gb.sound.playTick();
		}
		
		if (gb.buttons.pressed(BUTTON_A)) {
			gb.sound.playOK();
			detailedView();
			gridIndex = 0;
			loadGridView();
			cursorX = currentGame%2 ? 1 : 0;
			if (currentGame < 2) {
				cursorY = 0;
				cameraY = cameraY_actual = 1;
			} else {
				cursorY = 1;
				cameraY = cameraY_actual = -16;
			}
			continue; // else the next c-button-press will trigger
		}
		
		if (gb.buttons.pressed(BUTTON_C)) {
			gb.sound.playOK();
			settingsView();
		}
	}
}
