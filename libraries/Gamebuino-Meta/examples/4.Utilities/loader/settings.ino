extern const char LOADER_VERSION[];
const uint8_t settings_cursorPositions[] = {
	8,
	20,
	26,
	32,
};
const uint8_t settings_numCursorPositions = 4;

const uint8_t numLangEntries = 3;
const MultiLang langEntries[numLangEntries] = {
	{LANG_EN, "en"},
	{LANG_FR, "fr"},
	{LANG_DE, "de"},
};


void settingsView() {
	uint16_t bootloader_major = gb.bootloader.version() >> 16;
	uint8_t bootloader_minor = gb.bootloader.version() >> 8;
	uint8_t bootloader_patch = gb.bootloader.version();
	char defaultName[13];
	gb.getDefaultName(defaultName);
	uint8_t cursor = 0;
	
	uint8_t curLangIndex = 0;
	for (;(curLangIndex < numLangEntries) && (langEntries[curLangIndex].code != gb.language.getCurrentLang()); curLangIndex++);
	while(1) {
		if (!gb.update()) {
			continue;
		}
		gb.display.clear();
		const char* s = gb.language.get(lang_settings);
		gb.display.setColor(WHITE);
		gb.display.setCursor(40 - strlen(s)*2, 1);
		gb.display.print(s);
		gb.display.setCursor(0, 8);
		gb.display.setColor(BEIGE);
		gb.display.print(" ");
		gb.display.print(lang_settings_default_name);
		gb.display.print(":\n  ");
		gb.display.setColor(LIGHTBLUE);
		gb.display.println(defaultName);
		gb.display.setColor(BEIGE);
		gb.display.print(" ");
		gb.display.println(lang_settings_enter_bootloader);
		gb.display.print(" ");
		gb.display.print(lang_settings_language);
		gb.display.print(": ");
		gb.display.println(langEntries[curLangIndex].str);
		gb.display.print(" ");
		gb.display.println(lang_settings_back);
		
		if ((gb.frameCount%10) < 5) {
			gb.display.setColor(RED);
			gb.display.setCursor(0, settings_cursorPositions[cursor]);
			gb.display.print(">");
		}
		
		gb.display.setColor(GRAY);
		s = gb.language.get(lang_versions);
		gb.display.setCursor(40 - strlen(s)*2, 45);
		gb.display.println(s);
		gb.display.print(gb.language.get(lang_loader));
		gb.display.print(": ");
		gb.display.println(LOADER_VERSION);
		gb.display.print(gb.language.get(lang_bootloader));
		gb.display.print(": ");
		gb.display.print(bootloader_major);
		gb.display.print(".");
		gb.display.print(bootloader_minor);
		gb.display.print(".");
		gb.display.println(bootloader_patch);
		
		if (gb.buttons.pressed(BUTTON_A)) {
			switch(cursor) {
				case 0:
					// change default name
					gb.sound.playOK();
					gb.keyboard(defaultName, 13);
					gb.settings.set(SETTING_DEFAULTNAME, defaultName, 13);
					break;
				case 1:
					// enter bootloader mode
					gb.bootloader.enter();
					break;
				case 2:
					// language
					// we handle this a bit later
					break;
				case 3:
					// back
					gb.sound.playCancel();
					return; // return
			}
		}
		if (cursor == 2) {
			// language
			if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_RIGHT) || gb.buttons.pressed(BUTTON_LEFT)) {
				if (gb.buttons.pressed(BUTTON_LEFT)) {
					if (curLangIndex == 0) {
						curLangIndex = numLangEntries - 1;
					} else {
						curLangIndex--;
					}
				} else {
					curLangIndex++;
					if (curLangIndex >= numLangEntries){
						curLangIndex = 0;
					}
				}
				gb.settings.set(SETTING_LANGUAGE, (int32_t)langEntries[curLangIndex].code);
				gb.language.setCurrentLang(langEntries[curLangIndex].code);
				gb.sound.playTick();
			}
		}
		if (gb.buttons.repeat(BUTTON_UP, 8)) {
			if (cursor == 0) {
				cursor = settings_numCursorPositions - 1;
			} else {
				cursor--;
			}
			gb.sound.playTick();
		}
		if (gb.buttons.repeat(BUTTON_DOWN, 8)) {
			cursor++;
			if (cursor >= settings_numCursorPositions) {
				cursor = 0;
			}
			gb.sound.playTick();
		}
		if (gb.buttons.pressed(BUTTON_C)) {
			gb.sound.playCancel();
			return;
		}
	}
}
