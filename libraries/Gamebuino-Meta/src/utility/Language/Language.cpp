#include "Language.h"

namespace Gamebuino_Meta {

LangCode Language::currentLang = LangCode::en;

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

void Print_Language::print(const MultiLang* l, uint8_t num ) {
	print(Language::get(l, num));
}

void Print_Language::println(const MultiLang* l, uint8_t num ) {
	println(Language::get(l, num));
}

}; // Gamebuino_Meta
