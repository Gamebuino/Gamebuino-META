#include "Language.h"

namespace Gamebuino_Meta {

LangCode Language::getCurrentLang() {
	return currentLang;
}

void Language::setCurrentLang(LangCode c) {
	currentLang = c;
}

const char* Language::get(const MultiLang* l, uint8_t num) {
	const char* def = 0;
	for (uint8_t i = 0; i < num; i++) {
		if (l[i].code == currentLang) {
			return l[i].str;
		}
		if (!def) {
			def = l[i].str;
		} else if (l[i].code == LangCode::en) {
			def = l[i].str;
		}
	}
	return def;
}

}; // Gamebuino_Meta