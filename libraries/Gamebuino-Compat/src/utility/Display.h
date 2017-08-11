#ifndef _GAMEBUINO_COMPAT_DISPLAY_H_
#define _GAMEBUINO_COMPAT_DISPLAY_H_

// include Image.h from Gamebuino-Meta
#include <utility/Image.h>

namespace Gamebuino_Compat {

class Display : public Gamebuino_Meta::Image {
public:
	Display();
	void clear();
};

}; // namespace Gamebuino_Compat

using Gamebuino_Compat::Display;

#endif // _GAMEBUINO_COMPAT_DISPLAY_H_
