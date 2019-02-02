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

#include "BMP.h"
#include "../Misc.h"

namespace Gamebuino_Meta {

#if USE_SDFAT
void writeAsRGB(uint16_t b, File* f) {
	RGB888 c = rgb565Torgb888(b);
	f->write(c.b);
	f->write(c.g);
	f->write(c.r);
}
#endif // USE_SDFAT

BMP::BMP() {
	valid = false;
	upside_down = false;
}

BMP::BMP(Image* img, uint16_t _frames) {
	if (img->colorMode == ColorMode::index) {
		depth = 4;
	} else if (!img->transparentColor) {
		depth = 24;
	} else {
		depth = 32;
	}
	width = img->_width;
	height = img->_height;
	frames = _frames;
	upside_down = false;
}

#if USE_SDFAT
BMP::BMP(File* file, Image* img) {
	valid = false;
	file->rewind();
	if (f_read16(file) != 0x4D42) {
		// no valid BMP header
		return;
	}
	file->seekCur(4); // skip file size
	creatorBits = f_read32(file);
	image_offset = f_read32(file);
	uint32_t header_size = f_read32(file);
	width = f_read32(file);
#if STRICT_IMAGES
	if (img->_width && width > img->_width) {
		// BMP too large
		return;
	}
#endif
	img->_width = width;
	int32_t pixel_height = (int32_t)f_read32(file);
	upside_down = false;
	if (pixel_height < 0) {
		upside_down = true;
		pixel_height = -pixel_height;
	}
	
	if (f_read16(file) != 1) { // # planes, must always be 1
		return;
	}
	depth = f_read16(file);
	if (depth != 4 && depth != 24 && depth != 32) {
		// we can only load BMPs of depth 4 or 24 or 32
		return;
	}
	
#if STRICT_IMAGES
	if (img->_height) {
		height = img->_height;
		img->frames = frames = pixel_height / img->_height;
	} else {
		uint32_t tmp_width = img->_width;
		if (depth == 4) {
			tmp_width /= 2;
		} else {
			tmp_width *= 2;
		}
		uint16_t frames_ = tmp_width*pixel_height / img->bufferSize;
		if (frames_ < 1) {
			frames_ = 1;
		}
		img->frames = frames = frames_;
		img->_height = height = pixel_height / frames_;
	}
#else
	if (img->_height) {
		height = img->_height;
		img->frames = frames = pixel_height / img->_height;
	} else {
		img->_height = height = pixel_height;
		img->frames = frames = 1;
	}
#endif
	
	uint32_t compression = f_read32(file);
	bool createRGBAmasks = false;
	if (depth == 32) {
		createRGBAmasks = compression == 3;
	}
	if (compression != 0 && !createRGBAmasks) {
		// we can only load uncompressed BMPs
		return;
	}
	
	// we assume our color table so just ignore the one specified in the BMP
	
	
	if (depth == 4) {
		file->seekCur(12); // we ignore image size, x pixels and y pixels per meter
		uint8_t num_colors = f_read32(file);
		if (num_colors > 16) {
			num_colors = 16;
		}
		file->seekSet(header_size + 14);
		for (uint8_t i = 0; i < num_colors; i++) {
			uint8_t b = file->read();
			uint8_t g = file->read();
			uint8_t r = file->read();
			file->read(); // trash transparency
			indexMap[i] = (uint8_t)Graphics::rgb565ToIndex((Color)rgb888Torgb565({r, g, b}));
		}
		
		
		img->colorMode = ColorMode::index;
		img->useTransparentIndex = false; // TODO: transparency detection
	} else {
		if (createRGBAmasks) {
			file->seekCur(20); // we ignore mage size, x pixels per meter, y pixels per meter, colors and important colors
			for (uint8_t i = 0; i < 4; i++) {
				uint32_t mask = f_read32(file);
				for (uint8_t j = 0; j < 4; j++) {
					if ((0xFF << (j*8)) & mask) {
						indexMap[i] = j;
						break;
					}
				}
			}
		} else {
			indexMap[0] = 2;
			indexMap[1] = 1;
			indexMap[2] = 0;
			indexMap[3] = 3;
		}
		img->colorMode = ColorMode::rgb565;
		img->transparentColor = 0; // transparent color is detected during frame analysis
	}
	
	file->seekSet(image_offset);
	valid = true;
}
#endif // USE_SDFAT

bool BMP::isValid() {
	return valid;
}

uint32_t BMP::getCreatorBits() {
	return creatorBits;
}

#if USE_SDFAT
void BMP::setCreatorBits(uint32_t bits, File* file) {
	file->seekSet(6);
	f_write32(bits, file);
}
#endif // USE_SDFAT

#if USE_SDFAT
uint32_t BMP::writeHeader(File* file) {
	// lets first create some helpful variables
	uint16_t header_size = 40;
	uint32_t pixel_height = height * frames;
	uint8_t color_table = 0;
	if (depth == 4) {
		color_table = 16;
	}
	if (depth == 32) {
		header_size = 124;
	}
	image_offset = 14 + header_size + color_table * 4;
	uint32_t image_size = getRowSize() * pixel_height;
	uint32_t file_size = image_offset + image_size;
	
	
	file->rewind();
	file->write("BM"); // this actually is a BMP image
	f_write32(file_size, file);
	f_write32(0, file); // creator bits
	f_write32(image_offset, file);
	f_write32(header_size, file);
	f_write32(width, file);
	f_write32(pixel_height, file);
	f_write16(1, file); // number of panes must be 1
	f_write16(depth, file);
	if (depth == 32) {
		f_write32(3, file); // bitmask
	} else {
		f_write32(0, file); // no compression
	}
	f_write32(image_size, file);
	f_write32(0, file); // x pixels per meter horizontal
	f_write32(0, file); // y pixels per meter vertically
	f_write32(color_table, file);
	if (color_table) {
		f_write32(color_table, file); // important colors
		for (uint8_t i = 0; i < color_table; i++) {
			writeAsRGB((uint16_t)Graphics::colorIndex[i], file);
			file->write((uint8_t)0);
		}
	} else {
		f_write32(0, file); // no important colors
	}
	if (depth == 32) {
		f_write32(0x00FF0000, file); // R bitmask
		f_write32(0x0000FF00, file); // G bitmask
		f_write32(0x000000FF, file); // B bitmask
		f_write32(0xFF000000, file); // alpha bitmask
		f_write32(0x73524742, file); // color space sRGB
		for (uint8_t i = 0; i < ((0x24 / 4) + 3 + 4); i++) {
			f_write32(0, file); // unused stuff
		}
	}
	return image_size;
}
#endif // USE_SDFAT

uint32_t BMP::getRowSize() {
	if (depth == 4) {
		return ((4*width+31)/32) * 4;
	}
	if (depth == 24) {
		return (width * 3 + 3) & ~3;
	}
	return width * 4;
}

#if USE_SDFAT
uint16_t BMP::readBuffer(uint16_t* buf, uint32_t offset, uint16_t transparentColor, File* file) {
	
	uint32_t rowSize = getRowSize();
	file->seekSet(offset);
	if (depth == 4) {
		uint8_t dif = rowSize - ((width + 1) / 2);
		
		for (uint16_t i = 0; i < height; i++) {
			uint8_t* rambuffer;
			if (upside_down) {
				rambuffer = (uint8_t*)buf + i * ((width + 1) / 2);
			} else {
				rambuffer = (uint8_t*)buf + (height - 1 - i) * ((width + 1) / 2);
			}
			
			
			for (uint16_t j = 0; j < (width + 1)/2; j++) {
				uint8_t b = file->read();
				uint8_t u = b >> 4;
				uint8_t l = b & 0x0F;
				u = indexMap[u];
				l = indexMap[l];
				rambuffer[j] = (u << 4) | l;
			}
			file->seekCur(dif);
		}
	} else {
		uint8_t dif = rowSize - (width * 3);
		for (uint16_t i = 0; i < height; i++) {
			uint16_t* rambuffer;
			if (upside_down) {
				rambuffer = buf + i * width;
			} else {
				rambuffer = buf + (height - 1 - i) * width;
			}
			
			for (uint16_t j = 0; j < width; j++) {
				uint8_t c[4];
				if (depth == 24) {
					c[2] = file->read();
					c[1] = file->read();
					c[0] = file->read();
					c[3] = 0xFF; // assume no transparency
				} else {
					for (uint8_t k = 0; k < 4; k++) {
						c[indexMap[k]] = file->read();
					}
				}
				
				if (c[3] > 0x80) {
					uint16_t col = rgb888Torgb565({c[0], c[1], c[2]});
					if (transparentColor && col == transparentColor) {
						return transparentColor + 1;
					}
					rambuffer[j] = col;
				} else {
					// ok we have a transparent pixel
					if (!transparentColor) {
						return 1;
					}
					rambuffer[j] = transparentColor;
				}
			}
			if (depth != 32) { // 32-bit always has dif 0
				file->seekCur(dif);
			}
		}
	}
	return transparentColor;
}
#endif // USE_SDFAT

#if USE_SDFAT
uint16_t BMP::readFrame(uint16_t frame, uint16_t* buf, uint16_t transparentColor, File* file) {
	uint32_t size = getRowSize() * height;
	uint32_t offset;
	if (upside_down) {
		offset = size * frame;
	} else {
		offset = size * (frames - frame - 1);
	}
	return readBuffer(buf, image_offset + offset, transparentColor, file);
}
#endif // USE_SDFAT

#if USE_SDFAT
void BMP::writeBuffer(uint16_t* buffer, uint16_t transparentColor, File* file) {
	if (depth == 4) {
		uint8_t halfwidth = (width + 1) / 2;
		uint8_t j = getRowSize() - halfwidth;
		for (int8_t y = height - 1; y >= 0; y--) {
			uint8_t* buf = (uint8_t*)buffer + y*halfwidth;
			for (uint8_t x = 0; x < halfwidth; x++) {
				file->write(buf[x]);
			}
			uint8_t i = j;
			while (i--) {
				// time to add padding
				file->write((uint8_t)0);
			}
		}
	} else {
		uint8_t j = getRowSize() - (3*width);
		for (int8_t y = height - 1; y >= 0; y--) {
			uint16_t* buf = buffer + (y*width);
			for (uint8_t x = 0; x < width; x++) {
				uint16_t b = buf[x];
				if (depth == 24) {
					writeAsRGB(b, file);
				} else if (b == transparentColor) {
					f_write32(0x00000000, file);
				} else {
					writeAsRGB(b, file);
					file->write((uint8_t)0xFF);
				}
			}
			if (depth == 24) {
				uint8_t i = j;
				while (i--) {
					// time to add padding
					file->write((uint8_t)0);
				}
			}
		}
	}
}
#endif // USE_SDFAT

#if USE_SDFAT
void BMP::writeFrame(uint16_t frame, uint16_t* buf, uint16_t transparentColor, File* file) {
	uint32_t size = getRowSize() * height;
	uint32_t offset = size * (frames - frame - 1);
	file->seekSet(image_offset + offset);
	writeBuffer(buf, transparentColor, file);
}
#endif // USE_SDFAT

} // namespace Gamebuino_Meta
