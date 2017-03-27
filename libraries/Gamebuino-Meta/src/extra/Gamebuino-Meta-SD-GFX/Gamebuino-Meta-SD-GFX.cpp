#include "Gamebuino-Meta-SD-GFX.h"
#include "../SdFat.h"
extern SdFat SD;

Print* Gamebuino_SD_GFX::debugOutput = 0;

template <typename T>
inline void printDebug(T output){
	if(!Gamebuino_SD_GFX::debugOutput) return;
	Gamebuino_SD_GFX::debugOutput->print(output);
}

template <typename T>
inline void printlnDebug(T output){
	if(!Gamebuino_SD_GFX::debugOutput) return;
	Gamebuino_SD_GFX::debugOutput->println(output);
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
uint16_t read16(File f) {
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t read32(File f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

uint16_t convertTo565(uint8_t r, uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void write32(uint32_t b, File * f) {
	//Write four bytes
	//Be careful of the byte order!
	f->write(b & 0xFF); //LSB
	b >>= 8;
	f->write(b & 0xFF);
	b >>= 8;
	f->write(b & 0xFF);
	b >>= 8;
	f->write(b); //MSB
}

void write16(uint16_t b, File * f) {
	//Write two bytes
	//Be careful of the byte order!
	f->write(b & 0xFF); //LSB
	b >>= 8;
	f->write(b); //MSB
}


void Gamebuino_SD_GFX::begin(){
	
}

uint8_t Gamebuino_SD_GFX::writeImage(Image img, char *filename){
	printDebug("SAVING TO ");
	printlnDebug(filename);

	// let's first make sure that our file doesn't already exist
	if (SD.exists(filename)) {
		printDebug(" ALREADY EXISTS, REMOVING");
		if (SD.remove(filename)) {
			printlnDebug(" OK");
		} else {
			printlnDebug(" FAILED");
			return 1;
		}
	}

	printDebug(" CREATING FILE ");
	File file = SD.open(filename, FILE_WRITE);
	if (!file) {
		printlnDebug(" FAILED");
		return 2;
	}
	printlnDebug(" OK");
	
	uint8_t bmpDepth; // let's generate our bit-depth
#define BMP_HEADER_SIZE 40
	uint32_t bmpWidth; // this will hold the bmp width, padded to four bytes
	uint32_t colorTable = 0; // this is the color table, if needed
	uint16_t* rambuffer = img._buffer;
	switch(img.colorMode){
		case ColorMode::INDEX: 
			bmpDepth=4;
			bmpWidth = ((bmpDepth*img._width + 31)/32) * 4;
			colorTable = *(rambuffer++) | (*(rambuffer++)) << 16;
			
			printlnDebug("Indexed colors");
			break;
		case ColorMode::RGB565: 
			bmpDepth=24;
			bmpWidth = (img._width * 3 + 3) & ~3;
			
			printlnDebug("Full colors");
			break;
		default:
			printDebug("Invalid Image mode");
			return 255;
	}
	uint32_t bmpImageoffset = 14 + BMP_HEADER_SIZE + colorTable * 4; // here the image will start
	uint32_t bmpImageSize = bmpWidth*img._height; // this holds the image size in bytes
	uint32_t fileSize = bmpImageoffset + bmpImageSize; // this is the filesize
	
	// let's start writing the BMP header!
	file.write("BM"); // this actually is a BMP image
	printDebug('.');
	write32(fileSize, &file);
	write32(0, &file); // reserved
	printDebug('.');
	write32(bmpImageoffset, &file);
	printDebug('.');
	write32(BMP_HEADER_SIZE, &file);
	printDebug('.');
	write32(img._width, &file);
	printDebug('.');
	write32(img._height, &file);
	printDebug('.');
	write16(1, &file); // planes must be 1
	write16(bmpDepth, &file);
	write32(0, &file); // no compression
	printDebug('.');
	write32(bmpImageSize, &file);
	printDebug('.');
	write32(0, &file); // x pixels per meter horizontal
	write32(0, &file); // y pixels per meter vertical
	write32(colorTable, &file); // number of colors in the color table
	if (colorTable) {
		// we have a color table
		write32(*(rambuffer++) | ((*(rambuffer++)) << 16), &file); // important colors
		for (uint32_t i = 0; i < colorTable; i++) {
			write32(*(rambuffer++) | ((*(rambuffer++)) << 16), &file); // add the colors!
			printDebug('.');
		}
		// the header is done now, it is time to output the pixels!
		// we devide height*width by 2 as we output 2 pixels at once
		for (uint16_t i = 0; i < img._height*img._width/2; i++) {
			write16(*(rambuffer++), &file);
			//no padding is needed, as any padding is stored in rambuffer
		}
	} else {
		// we don't have a color table
		write32(0, &file); // no important colors
		printDebug(".\n");
		// the header is done now, it is time to output the pixels!
		uint8_t j = bmpWidth - (3*img._width);
		for (int8_t y = img._height - 1; y >= 0; y--) {
			uint16_t* buf = rambuffer + (y*img._width);
			for (uint8_t x = 0; x < img._width; x++) {
				uint16_t b = *(buf++); // fetch our buf
				
				// red
				uint8_t c = (uint8_t)(b << 3);
				c |= c >> 5;
				file.write(c);
				
				// green
				c = (uint8_t)((b >> 3) & 0xFC);
				c |= c >> 6;
				file.write(c);
				
				// blue
				c = (uint8_t)((b >> 8) & 0xF8);
				c |= c >> 5;
				file.write(c);
			}
			for (uint8_t i = j; i > 0; i++) {
				// time to add padding
				file.write((uint8_t)0);
			}
			printDebug('.');
		}
	}
	file.close();
	printDebug("\n DONE!");
	return 0;
}

uint8_t Gamebuino_SD_GFX::readImage(Image img, char *filename){
#if 0
	
	File     myFile;
	int      bmpWidth, bmpHeight;               // W+H in pixels
	uint16_t  bmpDepth;                          // Bit depth (must be 24 for rgb888)
										        // Can be 4 for 16 color index
	uint32_t headerSize;			            // Header size, to know where the color table begins
	uint32_t bmpImageoffset;                    // Start of image data in file
	uint32_t bmpColorTable, ImpColorCount;      // Number of colors in color table and Important color count
	uint32_t rowSize;                           // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[BUFFPIXEL/2];           // pixel buffer (R+G+B per pixel)
	uint8_t  buffidx = sizeof(sdbuffer);        // Current position in sdbuffer
	boolean  goodBmp = false;                   // Set to true on valid header parse
	boolean  flip = true;                       // BMP is stored bottom-to-top
	//uint16_t rambuffer[5 + MAXWIDHT*MAXHEIGHT]; // The buffer in RAM to be returned
	uint16_t* rambuffer = img._buffer;
	uint16_t  rambuffid = 0;                     // Current position in rambuffer
	int      w, h, row, col;
	uint8_t  r, g, b;                    
	uint32_t pos = 0, startTime = millis();

  printlnDebug("Opening file...");
	myFile = SD.open(filename);
	if (myFile) {
		printlnDebug(filename);

		// read from the file until there's nothing else in it:
		while (myFile.available()) {
			// Parse BMP header
			if (read16(myFile) == 0x4D42) { // BMP signature
				printDebug("File size: "); printlnDebug(read32(myFile));
				(void)read32(myFile); // Read & ignore creator bytes
				bmpImageoffset = read32(myFile); // Start of image data
				printDebug("Image Offset: "); printlnDebug(bmpImageoffset);
				
				// Read DIB header
				headerSize = read32(myFile);
				printDebug("Header size: "); printlnDebug(headerSize);
				bmpWidth = read32(myFile);
				bmpHeight = read32(myFile);
				if (read16(myFile) == 1) { // # planes -- must be '1'
					bmpDepth = read16(myFile); // bits per pixel
					printDebug("Bit Depth: "); printlnDebug(bmpDepth);
					printDebug("Image size: ");
					printDebug(bmpWidth);
					printDebug('x');
					printlnDebug(bmpHeight);
					
					//RGB565 uncompressed
					if ((bmpDepth == 24) && (read32(myFile) == 0)) { // 0 = uncompressed

						goodBmp = true; // Supported BMP format -- proceed!

						// BMP rows are padded (if needed) to 4-byte boundary
						rowSize = (bmpWidth * 3 + 3) & ~3;

						// If bmpHeight is negative, image is in top-down order.
						// This is not canon but has been observed in the wild.
						if (bmpHeight < 0) {
							bmpHeight = -bmpHeight;
							flip = false;
						}

						w = bmpWidth;
						h = bmpHeight;
						
						img.colorMode = ColorMode::RGB565;
						
						//img.freeBuffer();
						img.allocateBuffer(w,h);
						
						//read bitmap content
						for (row = 0; row<h; row++) { // For each scanline...

													  // Seek to start of scan line.  It might seem labor-
													  // intensive to be doing this on every line, but this
													  // method covers a lot of gritty details like cropping
													  // and scanline padding.  Also, the seek only takes
													  // place if the file position actually needs to change
													  // (avoids a lot of cluster math in SD library).
							if (flip) // Bitmap is stored top-to-bottom
								pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
							else     // Bitmap is stored bottom-to-top order (normal BMP)
								pos = bmpImageoffset + row * rowSize;
							if (myFile.position() != pos) { // Need seek?
								myFile.seek(pos);
								buffidx = sizeof(sdbuffer); // Force buffer reload
							}

							for (col = 0; col<w; col++) { // For each pixel...
								if (buffidx >= sizeof(sdbuffer)) {
									myFile.read(sdbuffer, sizeof(sdbuffer));
									buffidx = 0; // Set index to beginning
								}

								// Convert pixel from BMP format to bytes written in hex
								b = sdbuffer[buffidx++];
								g = sdbuffer[buffidx++];
								r = sdbuffer[buffidx++];
								/*printDebug(b, HEX);
								printDebug(g, HEX);
								printDebug(r, HEX);
								printDebug(" | ");*/

								//Conversion into rgb565
								rambuffer[rambuffid++] = convertTo565(r, g, b);

							} // end pixel
							//printlnDebug();
						} // end scanline
						printDebug("\nLoaded in ");
						printDebug(millis() - startTime);
						printlnDebug(" ms");
						
					//16 colors index, uncompressed
					}else if ((bmpDepth == 4) && (read32(myFile) == 0)) { // 0 = uncompressed
						//In this case, we should also return a color table that is a 16*4 bytes array
						//and with at least one color in it. To do so, we put the table in the buffer
						//before the corresponding pixel array.
						goodBmp = true; // Supported BMP format -- proceed!

						//We continue to parse the headers to store the data concerning the color table
						(void)read32(myFile); // Read & ignore image size
						(void)read32(myFile); // Read & ignore X pixels per meter
						(void)read32(myFile); // Read & ignore Y pixels per meter
						bmpColorTable = read32(myFile);
						printDebug("Colors in color table: "); printlnDebug(bmpColorTable);
						ImpColorCount = read32(myFile);
						printDebug("Important color count: "); printlnDebug(ImpColorCount);

						//Extract Color Table
						int i;
						uint32_t color;
						bmpColorTable = min(16,bmpColorTable);
						for (i = 0;i < bmpColorTable;i++) {
							(void)read32(myFile);         // read and ignore color table
						}
						/*for (i = 0;i < bmpColorTable;i++) {
							color = read32(myFile);
							rambuffer[rambuffid++] = ((uint16_t *)&color)[0];
							rambuffer[rambuffid++] = ((uint16_t *)&color)[1];
						}*/
						
						// BMP rows are padded (if needed) to 4-byte boundary
						rowSize = ((bmpDepth*bmpWidth+31)/32) * 4;

						// If bmpHeight is negative, image is in top-down order.
						// This is not canon but has been observed in the wild.
						if (bmpHeight < 0) {
							bmpHeight = -bmpHeight;
							flip = false;
						}

						w = bmpWidth;
						h = bmpHeight;
						
						img.colorMode = ColorMode::INDEX;
						
						//img.freeBuffer();
						img.allocateBuffer(w,h);
						
						for (row = 0; row<h; row++) { // For each scanline...

													  // Seek to start of scan line.  It might seem labor-
													  // intensive to be doing this on every line, but this
													  // method covers a lot of gritty details like cropping
													  // and scanline padding.  Also, the seek only takes
													  // place if the file position actually needs to change
													  // (avoids a lot of cluster math in SD library).
							if (flip) // Bitmap is stored top-to-bottom
								pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
							else     // Bitmap is stored bottom-to-top order (normal BMP)
								pos = bmpImageoffset + row * rowSize;
							if (myFile.position() != pos) { // Need seek?
								myFile.seek(pos);
								buffidx = sizeof(sdbuffer); // Force buffer reload
							}

							for (col = 0; col<w; col+=4) { // For each pixel...
								if (buffidx >= sizeof(sdbuffer)) {
									myFile.read(sdbuffer, sizeof(sdbuffer));
									buffidx = 0; // Set index to beginning
								}
								

								// With index 16, pixels are written on only 4 bits (one semi byte)
								// So when we store 2 bytes in rambuffer[rambuffid], we actually have 4 pixels in it
								//rambuffer[rambuffid] =  sdbuffer[buffidx++] | (sdbuffer[buffidx++]<<8) ;
								
								rambuffer[rambuffid] = (sdbuffer[buffidx]<<8);
								buffidx++;
								rambuffer[rambuffid] |= sdbuffer[buffidx] ;
								buffidx++;
								
								//SerialUSB.print(sdbuffer[buffidx],HEX);
								//rambuffer[rambuffid] =  (sdbuffer[buffidx]>>4) | (sdbuffer[buffidx]<<4) ;
								//buffidx++;
								//SerialUSB.print(sdbuffer[buffidx],HEX);
								//rambuffer[rambuffid] |=  ((sdbuffer[buffidx]>>4) | (sdbuffer[buffidx]<<4))<<8 ;
								//buffidx++;
								//Gamebuino_SD_GFX::debugOutput->print(rambuffer[rambuffid],HEX);
								rambuffid++;

							} // end pixel
							//printlnDebug();
							//SerialUSB.println();
						} // end scanline
						printDebug("\nLoaded in ");
						printDebug(millis() - startTime);
						printlnDebug(" ms");
					}// end goodBmp
				}//planes=/=1
			}

			if (!goodBmp) printlnDebug("BMP format not recognized.");
		}
		// close the file:
		myFile.close();
	}
	else {
		// if the file didn't open, print an error:
		printDebug("error opening ");
		printlnDebug(filename);
		//printlnDebug("=============================================================");
	}//if(myFile)
	//printlnDebug("=============================================================");
	//return rambuffer;
#endif
}
