/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2017

This is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL)
as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License (LGPL) for more details.

You should have received a copy of the GNU Lesser General Public
License (LGPL) along with the library.
If not, see <http://www.gnu.org/licenses/>.

Authors:
 - Sorunome
*/

#ifndef _GAMEBUINO_META_BOOTLOADER_H_
#define _GAMEBUINO_META_BOOTLOADER_H_

#include "../../config/config.h"

namespace Gamebuino_Meta {

class Bootloader {
public:
	uint32_t version();
	void game(const char* filename);
	void game(char* filename);
	void loader();
	void enter();
	void error(uint16_t e);
	void lock();
	void unlock();
	void flash_delete(uint32_t addr);
	void flash_write(uint32_t size, uint32_t *src_addr, uint32_t *dst_addr);
};

}; // namespace Gamebuino_Meta

#endif // _GAMEBUINO_META_BOOTLOADER_H_
