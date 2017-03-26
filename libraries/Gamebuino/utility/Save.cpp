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

#define SAVEHEADER_SIZE 10
#define SAVECONF_DEFAULT_BLOBSIZE 32

#define MIN(x, y) ((x < y) ? x : y)


SaveDefault::SaveDefault(uint16_t _i, uint8_t _type, uint8_t _ival) {
	i = _i;
	type = _type;
	if (type == SAVETYPE_INT) {
		val.ival = _ival;
	} else {
		val.ptr = 0;
		length = _ival;
	}
}
SaveDefault::SaveDefault(uint16_t _i, uint8_t _type, const void* _ptr, uint8_t _length) {
	i = _i;
	type = _type;
	val.ptr = _ptr;
	length = _length;
}

Save::Save(Adafruit_ST7735 *_tft) {
	tft = _tft;
}

void Save::error(const char *s) {
	tft->setCursor(0, 0);
	tft->setColor(RED, BLACK);
	tft->print("SAVE ERROR ");
	tft->setColor(WHITE, BLACK);
	tft->print(s);
	while(1);
}


SaveVar Save::getVarInfo(uint16_t i) {
	if (i > blocks) {
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
	exists = SD.exists(filename);
	f = SD.open(filename, FILE_WRITE);
	if (!f) {
		error("Can't create savefile");
		while(1);
	}
	open = true;
	if (!exists) {
		// the file doesn't exist yet, so let's create it
		blocks = SAVE_BLOCKNUM;
		f.write("asdf"); // four garbage bytes (checksum)
		f.write(&blocks, 2); // write the amount of blocks
		
		// +4 because of 4-byte payload size
		for (uint32_t i = 0; i < (5*SAVE_BLOCKNUM) + 4; i++) {
			f.write((uint8_t)0);
		}
		
		f.flush(); // make sure the file gets created
		f.rewind(); // rewind it so that we can read its properties
	}
	// the file already exists, so time to read some properties!
	f.seekSet(4); // TODO: check magic number matching
	f.read(&blocks, 2); // how many blocks do we have?
	f.read(&payload_size, 4); // let's grab the payload size!
	// TODO: check for block number miss-match
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
	f.seekSet(SAVEHEADER_SIZE + (5*blocks) + b);
	
	// now finally perform the read
	f.read(buf, size);
	return true;
}




void Save::_set(uint16_t i, uint32_t b) {
	f.seekSet(SAVEHEADER_SIZE + blocks + (4*i));
	f.write(&b, 4);
}

void Save::set(uint16_t i, int32_t num) {
	openFile();
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
}

void Save::newBlob(uint16_t i, uint8_t size) {
	// set the int-table pointer
	_set(i, (((uint32_t)size) << 24) | payload_size);
	
	// now fill the payload with zeros
	f.seekSet(SAVEHEADER_SIZE + (5*blocks) + payload_size);
	for(uint8_t j = 0; j < size; j++) {
		f.write((uint8_t)0);
	}
	
	// aaand increase the payload size
	payload_size += size;
	f.seekSet(6);
	f.write(&payload_size, 4);
}

void Save::set(uint16_t i, char* buf) {
	set(i, (void*)buf, strlen(buf));
}

void Save::set(uint16_t i, const char* buf) {
	set(i, (void*)buf, strlen(buf));
}

void Save::set(uint16_t i, const void* buf, uint8_t bufsize) {
	set(i, (void*)buf, bufsize);
}

void Save::set(uint16_t i, void* buf, uint8_t bufsize) {
	openFile();
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
		set(i, buf, bufsize);
		return;
	}
	size = MIN(size, bufsize);
	
	// get the pointer and seek the file there
	b &= 0x00FFFFFF;
	f.seekSet(SAVEHEADER_SIZE + (5*blocks) + b);
	
	// now finally perform the write
	f.write(buf, size);
	f.flush();
}



void Save::del(uint16_t i) {
	openFile();
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
		f.seekSet(SAVEHEADER_SIZE + (5*blocks) + b + size + j);
		f.read(&c, 1);
		f.seekSet(SAVEHEADER_SIZE + (5*blocks) + b + j);
		f.write(c);
	}
	
	// now all that is left to do is to adjust the payload and filesize
	payload_size -= size;
	f.seekSet(6);
	f.write(&payload_size, 4);
	f.truncate(SAVEHEADER_SIZE + (5*blocks) + payload_size);
	f.flush();
}
