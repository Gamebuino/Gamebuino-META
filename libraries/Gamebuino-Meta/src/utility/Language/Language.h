#ifndef _GAMEBUINO_META_LANGUAGE_H_
#define _GAMEBUINO_META_LANGUAGE_H_

#include "../../config/config.h"
#include "LangCode.h"
#include <Arduino.h>

namespace Gamebuino_Meta {

struct MultiLang {
	const LangCode code;
	const char* str;
};

class Language {
private:
	LangCode currentLang;
public:
	LangCode getCurrentLang();
	void setCurrentLang(LangCode c);
#if LANGUAGE_DEFAULT_SIZE
	const char* get(const MultiLang* l, uint8_t num = LANGUAGE_DEFAULT_SIZE);
#else
	const char* get(const MultiLang* l, uint8_t num);
#endif
	template<typename T, uint8_t N>
	const char* get(const T (&l) [N]) {
		return get(l, N);
	};
	template<typename T, uint8_t N>
	const char* _get(const T (&l) [N]) {
		return get(l, N);
	};
};


}; // namespace Gamebuino_Meta

using Gamebuino_Meta::MultiLang;

#endif // _GAMEBUINO_META_LANGUAGE_H_
