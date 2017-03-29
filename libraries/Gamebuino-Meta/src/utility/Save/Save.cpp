/*
 * (C) Copyright 2017 Sorunome. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */


#include "Save.h"

namespace Gamebuino_Meta {

#define SAVEHEADER_SIZE 10
#define SAVECONF_DEFAULT_BLOBSIZE 32
#define SAVEFILE_PAYLOAD_START (SAVEHEADER_SIZE + (blocks * 5))

#define MIN(x, y) ((x < y) ? x : y)


SaveDefault::SaveDefault(uint16_t _i, uint8_t _type, int32_t _ival) {
	i = _i;
	type = _type;
	if (type == SAVETYPE_INT) {
		val.ival = _ival;
	} else {
		val.ptr = 0;
		length = (uint8_t)_ival;
	}
}
SaveDefault::SaveDefault(uint16_t _i, uint8_t _type, const void* _ptr, uint8_t _length) {
	i = _i;
	type = _type;
	val.ptr = _ptr;
	length = _length;
}

Save::Save(Display_ST7735 *_tft, const char* _checkbytes) {
	tft = _tft;
	checkbytes = _checkbytes;
}

void Save::error(const char *s) {
	tft->setCursor(0, 0);
	tft->setColor(Color::red, Color::black);
	tft->print("SAVE ERROR ");
	tft->setColor(Color::white, Color::black);
	tft->print(s);
	while(1);
}


SaveVar Save::getVarInfo(uint16_t i) {
	if (i >= blocks) {
		// Trying to access bad block...
		error("accessing non-existing block");
	}
	SaveVar s;
	f.seekSet(SAVEHEADER_SIZE + i);
	uint8_t b;
	if (!f.read(&b, 1)) {
		error("file I/O");
		while(1);
	}
	s.defined = (b & 0x80) ? true : false;
	s.type = b & 0x07;
	return s;
}

void Save::openFile() {
	if (open) {
		return;
	}
	const char filename[] = SAVEFILE_NAME;
	bool exists = SD.exists(filename);
	f = SD.open(filename, FILE_WRITE);
	if (!f) {
		// eeeeeh, can't open it so we are read-only
		open = true;
		readOnly = true;
		blocks = SAVEBLOCK_NUM;
		return;
	}
	open = true;
	if (!exists) {
		blocks = SAVEBLOCK_NUM;
		
		// the file doesn't exist yet, so let's create it
		f.write(checkbytes, 4);
		f.write(&blocks, 2); // write the amount of blocks
		
		// +4 because of 4-byte payload size
		for (uint32_t i = 0; i < (5*SAVEBLOCK_NUM) + 4; i++) {
			f.write((uint8_t)0);
		}
		
		f.flush(); // make sure the file gets created
	}
	f.rewind(); // rewind it so that we can read its properties
	// the file already exists, so time to read some properties!
	
	// first check that the checkbytes match!
	// we use the payload_size for this because that is just yet another free 4-byte buffer
	f.read(&payload_size, 4);
	if ((uint32_t)payload_size ^ *((uint32_t*)checkbytes)) {
		error("Invalid save file");
	}
	
	f.read(&blocks, 2); // how many blocks do we have?
	f.read(&payload_size, 4); // let's grab the payload size!
	
	if (blocks == SAVEBLOCK_NUM) {
		return; // everything is OK, nothing more to do
	}
	if (blocks > SAVEBLOCK_NUM) {
		// we need to shrink the block size
		
		// first we delete the unneeded variables (due to payload maybe being off)
		for (uint16_t i = SAVEBLOCK_NUM; i < blocks; i++) {
			del(i);
		}
		
		// next we offset the blocks
		for (uint16_t i = 0; i < SAVEBLOCK_NUM; i++) {
			uint32_t b;
			f.seekSet(SAVEHEADER_SIZE + blocks + i*4);
			f.read(&b, 4);
			f.seekSet(SAVEHEADER_SIZE + SAVEBLOCK_NUM + i*4);
			f.write(&b, 4);
		}
		
		// now we fix the payload
		for (uint32_t i = 0; i < payload_size; i++) {
			uint8_t b;
			f.seek(SAVEHEADER_SIZE + (blocks * 5) + i);
			f.read(&b, 1);
			f.seek(SAVEHEADER_SIZE + (SAVEBLOCK_NUM * 5) + i);
			f.write(&b, 1);
		}
		f.truncate(SAVEHEADER_SIZE + (SAVEBLOCK_NUM * 5) + payload_size);
	} else {
		// we need to grow the block size
		
		// first we grow the file by the desired amount
		f.seekSet(SAVEHEADER_SIZE + (blocks * 5) + payload_size);
		for (uint32_t i = 0; i < (SAVEBLOCK_NUM - blocks)*5; i++) {
			f.write((uint8_t)0);
		}
		
		// next we shift the payload back
		for (uint32_t i = 0; i < payload_size; i++) {
			uint8_t b;
			f.seek(SAVEHEADER_SIZE + (blocks * 5) + (payload_size - i - 1));
			f.read(&b, 1);
			f.seek(SAVEHEADER_SIZE + (SAVEBLOCK_NUM * 5) + (payload_size - i - 1));
			f.write(&b, 1);
		}
		
		// next we offset the blocks
		for (uint16_t i = 0; i < blocks; i++) {
			uint32_t b;
			f.seekSet(SAVEHEADER_SIZE + blocks + (blocks - i - 1)*4);
			f.read(&b, 4);
			f.seekSet(SAVEHEADER_SIZE + SAVEBLOCK_NUM + (blocks - i - 1)*4);
			f.write(&b, 4);
		}
		
		// finally we nullate the new block metadata
		f.seekSet(SAVEHEADER_SIZE + blocks);
		for (uint16_t i = blocks; i < SAVEBLOCK_NUM; i++) {
			f.write((uint8_t)0);
		}
	}
	blocks = SAVEBLOCK_NUM;
	f.seekSet(4);
	f.write(&blocks, 2);
	f.flush();
}




uint32_t Save::_get(uint16_t i) {
	uint32_t val;
	f.seekSet(SAVEHEADER_SIZE + blocks + (4*i));
	f.read(&val, 4);
	return val;
}

int32_t Save::get(uint16_t i) {
	openFile();
	SaveVar s = getVarInfo(i);
	if (!s.defined) {
		for (uint16_t j = 0; j < SAVECONF_SIZE; j++) {
			if (defaults[j].i == i) {
				// we found our element!
				if (defaults[j].type != SAVETYPE_INT) {
					error("trying to get from a non-int type");
				}
				return defaults[j].val.ival;
			}
		}
		return 0;
	}
	if (s.type != SAVETYPE_INT) {
		error("trying to get from a non-int type");
	}
	return (int32_t)_get(i);
}

bool Save::get(uint16_t i, void* buf, uint8_t bufsize) {
	openFile();
	SaveVar s = getVarInfo(i);
	if (!s.defined) {
		memset(buf, 0, bufsize);
		for (uint16_t j = 0; j < SAVECONF_SIZE; j++) {
			if (defaults[j].i == i) {
				// we found our element!
				if (defaults[j].type != SAVETYPE_BLOB) {
					error("trying to get from a non-blob type");
				}
				if (defaults[j].val.ptr) {
					memcpy(buf, defaults[j].val.ptr, MIN(bufsize, defaults[j].length));
				}
				break;
			}
		}
		return false;
	}
	if (s.type != SAVETYPE_BLOB) {
		error("trying to get from a non-blob type");
	}
	
	uint32_t b = _get(i);
	
	// determine how many bytes to set
	uint8_t size = b >> 24;
	size = MIN(size, bufsize);
	
	// get the pointer and seek the file there
	b &= 0x00FFFFFF;
	f.seekSet(SAVEFILE_PAYLOAD_START + b);
	
	// now finally perform the read
	f.read(buf, size);
	return true;
}




void Save::_set(uint16_t i, uint32_t b) {
	f.seekSet(SAVEHEADER_SIZE + blocks + (4*i));
	f.write(&b, 4);
}

bool Save::set(uint16_t i, int32_t num) {
	openFile();
	if (readOnly) {
		return false;
	}
	SaveVar s = getVarInfo(i);
	if (s.defined && s.type != SAVETYPE_INT) {
		// trying to store an int in a non-int
		error("trying set to a non-int type");
	}
	for (uint16_t j = 0; j < SAVECONF_SIZE; j++) {
		if (defaults[j].i == i) {
			// we found our element!
			if (defaults[j].type != SAVETYPE_INT) {
				error("trying set to a non-int type");
			}
			break;
		}
	}
	if (!s.defined) {
		f.seekSet(SAVEHEADER_SIZE + i);
		f.write((uint8_t)(0x80 | SAVETYPE_INT));
	}
	_set(i, (uint32_t)num);
	f.flush();
	return true;
}

void Save::newBlob(uint16_t i, uint8_t size) {
	// set the int-table pointer
	_set(i, (((uint32_t)size) << 24) | payload_size);
	
	// now fill the payload with zeros
	f.seekSet(SAVEFILE_PAYLOAD_START + payload_size);
	for(uint8_t j = 0; j < size; j++) {
		f.write((uint8_t)0);
	}
	
	// aaand increase the payload size
	payload_size += size;
	f.seekSet(6);
	f.write(&payload_size, 4);
}

bool Save::set(uint16_t i, char* buf) {
	return set(i, (void*)buf, strlen(buf));
}

bool Save::set(uint16_t i, const char* buf) {
	return set(i, (void*)buf, strlen(buf));
}

bool Save::set(uint16_t i, const void* buf, uint8_t bufsize) {
	return set(i, (void*)buf, bufsize);
}

bool Save::set(uint16_t i, void* buf, uint8_t bufsize) {
	openFile();
	if (readOnly) {
		return false;
	}
	SaveVar s = getVarInfo(i);
	if (s.defined && s.type != SAVETYPE_BLOB) {
		// trying to store an int in a non-int
		error("trying set to a non-blob type");
	}
	uint8_t want_size = SAVECONF_DEFAULT_BLOBSIZE;
	for (uint16_t j = 0; j < SAVECONF_SIZE; j++) {
		if (defaults[j].i == i) {
			// we found our element!
			if (defaults[j].type != SAVETYPE_BLOB) {
				error("trying set to a non-blob type");
			}
			want_size = defaults[j].length;
			break;
		}
	}
	
	if (!s.defined) {
		// first we create the blob entry
		f.seekSet(SAVEHEADER_SIZE + i);
		f.write((uint8_t)(0x80 | SAVETYPE_BLOB));
		newBlob(i, want_size);
	}
	
	uint32_t b = _get(i);
	
	// determine how many bytes to set
	uint8_t size = b >> 24;
	if (size != want_size) {
		// ok the size is different, so let's change this!
		del(i);
		return set(i, buf, bufsize);
	}
	size = MIN(size, bufsize);
	
	// get the pointer and seek the file there
	b &= 0x00FFFFFF;
	f.seekSet(SAVEFILE_PAYLOAD_START + b);
	
	// now finally perform the write
	f.write(buf, size);
	f.flush();
	return true;
}



void Save::del(uint16_t i) {
	openFile();
	if (readOnly) {
		return;
	}
	SaveVar s = getVarInfo(i);
	if (!s.defined) {
		return; // nothing to do!
	}
	
	// let's delete the entry first
	f.seekSet(SAVEHEADER_SIZE + i);
	f.write((uint8_t)0);
	
	if (s.type == SAVETYPE_INT) {
		f.flush();
		return; // with ints that is all what is left to do
	}
	
	// ok, we have a blob, so we must get rid of the payload...
	uint32_t b = _get(i);
	
	// determine the size of the payload
	uint8_t size = b >> 24;
	
	// get the pointer and seek the file there
	b &= 0x00FFFFFF;
	
	// now we need to loop all blocks and shift those with a payload pointer that is greater down a bit
	for (i = 0; i < blocks; i++) {
		s = getVarInfo(i);
		if (s.defined && s.type == SAVETYPE_BLOB) {
			uint32_t c = _get(i);
			uint8_t size_c = c >> 24;
			c &= 0x00FFFFFF;
			if (c > b) {
				// we just adjust the pointer here, we do all the shifting later on
				c -= size;
				c |= (size_c << 24);
				_set(i, c);
			}
		}
	}
	// ok now we actually need to shift the payload data
	for (uint32_t j = 0; j < (payload_size-b-size); j++) {
		uint8_t c;
		f.seekSet(SAVEFILE_PAYLOAD_START + b + size + j);
		f.read(&c, 1);
		f.seekSet(SAVEFILE_PAYLOAD_START + b + j);
		f.write(c);
	}
	
	// now all that is left to do is to adjust the payload and filesize
	payload_size -= size;
	f.seekSet(6);
	f.write(&payload_size, 4);
	f.truncate(SAVEFILE_PAYLOAD_START + payload_size);
	f.flush();
}

} // Gamebuino_Meta
