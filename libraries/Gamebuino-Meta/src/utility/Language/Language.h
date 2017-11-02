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
	static LangCode currentLang;
public:
	static LangCode getCurrentLang();
	static void setCurrentLang(LangCode c);
#if LANGUAGE_DEFAULT_SIZE
	static const char* get(const MultiLang* l, uint8_t num = LANGUAGE_DEFAULT_SIZE);
#else
	static const char* get(const MultiLang* l, uint8_t num);
#endif
	template<uint8_t N>
	static const char* get(const MultiLang (&l) [N]) {
		return get(l, N);
	};
	template<uint8_t N>
	static const char* _get(const MultiLang (&l) [N]) {
		return get(l, N);
	};
};



class Print_Language : public Print {
public:
	using Print::print;
	using Print::println;
	using Print::printf;
#if LANGUAGE_DEFAULT_SIZE
	void print(const MultiLang* l, uint8_t num = LANGUAGE_DEFAULT_SIZE);
	void println(const MultiLang* l, uint8_t num = LANGUAGE_DEFAULT_SIZE);
#else
	void print(const MultiLang* l, uint8_t num);
	void println(const MultiLang* l, uint8_t num);
#endif
	template <uint8_t N>
	void print(const MultiLang (&l) [N]) {
		print(l, N);
	};
	template <uint8_t N>
	void println(const MultiLang (&l) [N]) {
		println(l, N);
	};
	template <uint8_t N>
	void printf(const MultiLang (&l) [N], ...) {
		char buf[PRINTF_BUF];
		va_list ap;
		const char* format = Language::get(l, N);
		va_start(ap, format);
		vsnprintf(buf, sizeof(buf), format, ap);
		write(buf);
		va_end(ap);
	};
};


}; // namespace Gamebuino_Meta

using Gamebuino_Meta::MultiLang;

#endif // _GAMEBUINO_META_LANGUAGE_H_
