#include "Gamebuino-SD-GFX.h"

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

void write32(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, File f) {
	//Write four bytes
	//Be careful of the byte order!
	f.write(byte1); //LSB
	f.write(byte2);
	f.write(byte3);
	f.write(byte4); //MSB
}

void write16(uint8_t byte1, uint8_t byte2, File f) {
	//Write two bytes
	//Be careful of the byte order!
	f.write(byte1); //LSB
	f.write(byte2); //MSB
}


void Gamebuino_SD_GFX::begin(){
	
}

uint8_t Gamebuino_SD_GFX::writeImage(Image img, char *filename){
	File file;
	uint8_t bmpDepth = 24;        // value for rgb 888
	uint32_t width, height;       // image size (w+h in pixels)
	uint32_t headerSize = 40;     // default value (ignoring bitmasks)
	uint32_t rowSize;             // Not always = width; may have padding
	uint32_t fileSize;            // We have to know the file size before completing the headers
	uint32_t bmpImageoffset;      // Start of image data in file
	uint32_t rawImageSize;        // Image Size including padding
	uint32_t colorTable = 0;      // Number of colors in the palette
	int row, col, rambuffind;    
	uint32_t startTime = millis();
	uint16_t* rambuffer = img._buffer;

  SerialUSB.println("Checking if file exists");
	if (SD.exists(filename)) {
		SerialUSB.print(filename );
		SerialUSB.println(" already exists.");
		// delete the file:
		SerialUSB.print("Removing ");
		SerialUSB.println(filename);
		if (SD.remove(filename)) SerialUSB.println("Removal success");
		else {
			SerialUSB.println("ERROR in removing the file!");
			return 1;
		}
	}
	SerialUSB.print("Creating ");
	SerialUSB.println(filename );
	file = SD.open(filename, FILE_WRITE);
	if (file) {
		//Color depth, width and height info
		//rambuffind = 0;
		//bmpDepth = rambuffer[rambuffind++];
		//width = rambuffer[rambuffind++] | (rambuffer[rambuffind++] << 16);
		//height = rambuffer[rambuffind++] | (rambuffer[rambuffind++] << 16);
		rambuffind = 0;
		bmpDepth = 24;
		width = img._width;
		height = img._height;
		
		SerialUSB.print("Bit Depth: "); SerialUSB.println(bmpDepth);
		SerialUSB.print("Image size: ");
		SerialUSB.print(width);
		SerialUSB.print('x');
		SerialUSB.println(height);

		if (bmpDepth == 24) {//the file is a rgb888

			//BMP Header : BMP header size is 14 bytes
			write16(0x42, 0x4D, file); // BMP signature
			SerialUSB.print('.');
			rowSize = (width * 3 + 3) & ~3; // BMP rows are padded (if needed) to 4-byte boundary
			rawImageSize = rowSize*height;
			bmpImageoffset = 14 + headerSize;
			fileSize = bmpImageoffset + rawImageSize;
			write32(((uint8_t *)&fileSize)[0], ((uint8_t *)&fileSize)[1],
				((uint8_t *)&fileSize)[2], ((uint8_t *)&fileSize)[3], file); //BMP file size
			write16(0, 0, file); //Reserved
			write16(0, 0, file); //Reserved
			SerialUSB.print('.');
			write32(((uint8_t *)&bmpImageoffset)[0], ((uint8_t *)&bmpImageoffset)[1],
				((uint8_t *)&bmpImageoffset)[2], ((uint8_t *)&bmpImageoffset)[3], file); //BMP pixel array offset
			SerialUSB.print('.');

			//DIB Header : DIB header size is 40 
			write32(((uint8_t *)&headerSize)[0], ((uint8_t *)&headerSize)[1],
				((uint8_t *)&headerSize)[2], ((uint8_t *)&headerSize)[3], file); //BMP header Size
			SerialUSB.print('.');
			write32(((uint8_t *)&width)[0], ((uint8_t *)&width)[1],
				((uint8_t *)&width)[2], ((uint8_t *)&width)[3], file); //BMP Widht
			write32(((uint8_t *)&height)[0], ((uint8_t *)&height)[1],
				((uint8_t *)&height)[2], ((uint8_t *)&height)[3], file); //BMP Height
			SerialUSB.print('.');
			write16(1, 0, file); //Planes must be 1
			write16(bmpDepth, 0, file); //BMP Depth = bits per pixel
			write32(0, 0, 0, 0, file); //No compression = 0
			SerialUSB.print('.');
			write32(((uint8_t *)&rawImageSize)[0], ((uint8_t *)&rawImageSize)[1],
				((uint8_t *)&rawImageSize)[2], ((uint8_t *)&rawImageSize)[3], file); //Raw Image Size (including padding)
			SerialUSB.print('.');
			write32(0, 0, 0, 0, file); //X pixels per meter horizontal
			write32(0, 0, 0, 0, file); //Y pixels per meter vertical
			write32(0, 0, 0, 0, file); //Number of colors in the palette
			write32(0, 0, 0, 0, file); //Important color count
			SerialUSB.print('.');

			//Pixel Array
			for (row = 0;row < height;row++) {//each row of the array
				for (col = 0;col < width;col++) {//each pixel
					file.write((rambuffer[rambuffind] << 3));           //r
					file.write((rambuffer[rambuffind] >> 3) & 0xfc);    //g
					file.write((rambuffer[rambuffind++] >> 8) & 0xf8);  //b
					SerialUSB.print('.');
				}
				for (col =3*width;col < rowSize;col++) {//padding with zeros
					file.write((uint8_t)0);
					SerialUSB.print('.');
				}
			}
		}else if(bmpDepth==4) { //index 16, there must be an color table

			colorTable = rambuffer[rambuffind++] | (rambuffer[rambuffind++] << 16);

			//BMP Header : BMP header size is 14 bytes
			write16(0x42, 0x4D, file); // BMP signature
			SerialUSB.print('.');
			rowSize = ((bmpDepth*width + 31)/32) * 4; // BMP rows are padded (if needed) to 4-byte boundary
			rawImageSize = rowSize*height;
			bmpImageoffset = 14 + headerSize + colorTable * 4;
			fileSize = bmpImageoffset + rawImageSize;
			write32(((uint8_t *)&fileSize)[0], ((uint8_t *)&fileSize)[1],
				((uint8_t *)&fileSize)[2], ((uint8_t *)&fileSize)[3], file); //BMP file size
			write16(0, 0, file); //Reserved
			write16(0, 0, file); //Reserved
			SerialUSB.print('.');
			write32(((uint8_t *)&bmpImageoffset)[0], ((uint8_t *)&bmpImageoffset)[1],
				((uint8_t *)&bmpImageoffset)[2], ((uint8_t *)&bmpImageoffset)[3], file); //BMP pixel array offset
			SerialUSB.print('.');

			//DIB Header : DIB header size is 40 
			write32(((uint8_t *)&headerSize)[0], ((uint8_t *)&headerSize)[1],
				((uint8_t *)&headerSize)[2], ((uint8_t *)&headerSize)[3], file); //BMP header Size
			SerialUSB.print('.');
			write32(((uint8_t *)&width)[0], ((uint8_t *)&width)[1],
				((uint8_t *)&width)[2], ((uint8_t *)&width)[3], file); //BMP Widht
			write32(((uint8_t *)&height)[0], ((uint8_t *)&height)[1],
				((uint8_t *)&height)[2], ((uint8_t *)&height)[3], file); //BMP Height
			SerialUSB.print('.');
			write16(1, 0, file); //Planes must be 1
			write16(bmpDepth, 0, file); //BMP Depth = bits per pixel
			write32(0, 0, 0, 0, file); //No compression = 0
			SerialUSB.print('.');
			write32(((uint8_t *)&rawImageSize)[0], ((uint8_t *)&rawImageSize)[1],
				((uint8_t *)&rawImageSize)[2], ((uint8_t *)&rawImageSize)[3], file); //Raw Image Size (including padding)
			SerialUSB.print('.');
			write32(0, 0, 0, 0, file); //X pixels per meter horizontal
			write32(0, 0, 0, 0, file); //Y pixels per meter vertical
			write32(((uint8_t *)&colorTable)[0], ((uint8_t *)&colorTable)[1],
				((uint8_t *)&colorTable)[2], ((uint8_t *)&colorTable)[3], file); //Number of colors in the palette
			uint32_t impColor = 0;  // Important color count
			impColor = rambuffer[rambuffind++] | (rambuffer[rambuffind++] << 16);
			write32(((uint8_t *)&impColor)[0], ((uint8_t *)&impColor)[1],
				((uint8_t *)&impColor)[2], ((uint8_t *)&impColor)[3], file); //Important color count
			SerialUSB.print('.');

			// Color Palette
			int i;
			for (i = 0;i < colorTable;i++) { // we use impColor in order not to create a new variable
				impColor = rambuffer[rambuffind++] | (rambuffer[rambuffind++] << 16);
				write32(((uint8_t *)&impColor)[0], ((uint8_t *)&impColor)[1],
					((uint8_t *)&impColor)[2], ((uint8_t *)&impColor)[3], file);  //each color is added to the palette
				SerialUSB.print('.');
			}

			//Pixel Array
			for (row = 0;row < height;row++) {//each row of the array
				for (col = 0;col < width;col+=4) {//write 4 pixels in a row
					file.write(rambuffer[rambuffind]);
					file.write(rambuffer[rambuffind++] >> 8);
					SerialUSB.print('.');
				}//no padding is needed, as any padding is stored in rambuffer
			}
		}//end if(bmpDepth==24)
		SerialUSB.println();
		SerialUSB.print("written in ");
		SerialUSB.print(millis() - startTime);
		SerialUSB.println(" ms");

		//Close the file
		file.close();
	}
	else {
		SerialUSB.print("Error opening ");
		SerialUSB.println(filename);
		return 2;
	}

	return 0; //Success
}

uint8_t Gamebuino_SD_GFX::readImage(Image img, char *filename){
	
	File     myFile;
	int      bmpWidth, bmpHeight;               // W+H in pixels
	uint16_t  bmpDepth;                          // Bit depth (must be 24 for rgb888)
										        // Can be 4 for 16 color index
	uint32_t headerSize;			            // Header size, to know where the color table begins
	uint32_t bmpImageoffset;                    // Start of image data in file
	uint32_t bmpColorTable, ImpColorCount;      // Number of colors in color table and Important color count
	uint32_t rowSize;                           // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3 * BUFFPIXEL];           // pixel buffer (R+G+B per pixel)
	uint8_t  buffidx = sizeof(sdbuffer);        // Current position in sdbuffer
	boolean  goodBmp = false;                   // Set to true on valid header parse
	boolean  flip = true;                       // BMP is stored bottom-to-top
	//uint16_t rambuffer[5 + MAXWIDHT*MAXHEIGHT]; // The buffer in RAM to be returned
	uint16_t* rambuffer = img._buffer;
	int      rambuffid = 0;                     // Current position in rambuffer
	int      w, h, row, col;
	uint8_t  r, g, b;                    
	uint32_t pos = 0, startTime = millis();

  SerialUSB.println("Opening file...");
	myFile = SD.open(filename);
	if (myFile) {
		SerialUSB.println(filename);

		// read from the file until there's nothing else in it:
		while (myFile.available()) {
			// Parse BMP header
			if (read16(myFile) == 0x4D42) { // BMP signature
				SerialUSB.print("File size: "); SerialUSB.println(read32(myFile));
				(void)read32(myFile); // Read & ignore creator bytes
				bmpImageoffset = read32(myFile); // Start of image data
				SerialUSB.print("Image Offset: "); SerialUSB.println(bmpImageoffset, DEC);
				
				// Read DIB header
				headerSize = read32(myFile);
				SerialUSB.print("Header size: "); SerialUSB.println(headerSize);
				bmpWidth = read32(myFile);
				bmpHeight = read32(myFile);
				if (read16(myFile) == 1) { // # planes -- must be '1'
					bmpDepth = read16(myFile); // bits per pixel
					SerialUSB.print("Bit Depth: "); SerialUSB.println(bmpDepth);
					SerialUSB.print("Image size: ");
					SerialUSB.print(bmpWidth);
					SerialUSB.print('x');
					SerialUSB.println(bmpHeight);
					
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
								/*SerialUSB.print(b, HEX);
								SerialUSB.print(g, HEX);
								SerialUSB.print(r, HEX);
								SerialUSB.print(" | ");*/

								//Conversion into rgb565
								rambuffer[rambuffid++] = convertTo565(r, g, b);

							} // end pixel
							//SerialUSB.println();
						} // end scanline
						SerialUSB.println();
						SerialUSB.print("Loaded in ");
						SerialUSB.print(millis() - startTime);
						SerialUSB.println(" ms");
						
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
						SerialUSB.print("Colors in color table: "); SerialUSB.println(bmpColorTable);
						ImpColorCount = read32(myFile);
						SerialUSB.print("Important color count: "); SerialUSB.println(ImpColorCount);

						//Extract Color Table
						int i;
						uint32_t color;
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
						for (row = 0; row<h; row++) { // For each scanline...

													  // Seek to start of scan line.  It might seem labor-
													  // intensive to be doing this on every line, but this
													  // method covers a lot of gritty details like cropping
													  // and scanline padding.  Also, the seek only takes
													  // place if the file position actually needs to change
													  // (avoids a lot of cluster math in SD library).
							if (!flip) // Bitmap is stored top-to-bottom
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
								rambuffer[rambuffid++] = (sdbuffer[buffidx++]<<8) | sdbuffer[buffidx++];

							} // end pixel
							//SerialUSB.println();
						} // end scanline
						SerialUSB.println();
						SerialUSB.print("Loaded in ");
						SerialUSB.print(millis() - startTime);
						SerialUSB.println(" ms");
					}// end goodBmp
				}//planes=/=1
			}

			if (!goodBmp) SerialUSB.println("BMP format not recognized.");
		}
		// close the file:
		myFile.close();
	}
	else {
		// if the file didn't open, print an error:
		SerialUSB.print("error opening ");
		SerialUSB.println(filename);
		SerialUSB.println("=============================================================");
	}//if(myFile)
	SerialUSB.println("=============================================================");
	//return rambuffer;
}


