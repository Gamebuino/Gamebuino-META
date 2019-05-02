#include <Gamebuino-Meta.h>

const size_t BUF_SIZE = 512;
const uint32_t FILE_SIZE_MB = 5;
const uint8_t WRITE_COUNT = 2;
const uint8_t READ_COUNT = 2;
const uint32_t FILE_SIZE = 1000000UL*FILE_SIZE_MB;
uint8_t buf[BUF_SIZE];


// modified from https://github.com/greiman/SdFat/blob/master/examples/bench/bench.ino

char name[50];
void setup() {
	gb.begin();
	
	float s;
	uint32_t t;
	uint32_t maxLatency;
	uint32_t minLatency;
	uint32_t totalLatency;
	
	File file = SD.open("bench.dat", O_CREAT | O_TRUNC | O_RDWR);
	if (!file) {
		gb.tft.println("Could not create bench file");
		while(1);
	}
	for (size_t i = 0; i < (BUF_SIZE-2); i++) {
		buf[i] = 'A' + (i % 26);
	}
	buf[BUF_SIZE-2] = '\r';
	buf[BUF_SIZE-1] = '\n';
	uint32_t n = FILE_SIZE/sizeof(buf);
	gb.tft.clear();
	gb.tft.println("Write");
	for (uint8_t nTest = 0; nTest < WRITE_COUNT; nTest++) {
		file.truncate(0);
		maxLatency = 0;
		minLatency = 9999999;
		totalLatency = 0;
		t = millis();
		for (uint32_t i = 0; i < n; i++) {
			uint32_t m = micros();
			if (file.write(buf, sizeof(buf)) != sizeof(buf)) {
				gb.tft.print("WRITE ERROR");
				while(1);
			}
			m = micros() - m;
			if (maxLatency < m) {
				maxLatency = m;
			}
			if (minLatency > m) {
				minLatency = m;
			}
			totalLatency += m;
		}
		file.sync();
		t = millis() - t;
		s = file.fileSize();
		gb.tft.print("Speed:");
		gb.tft.print(s/t);
		gb.tft.println("KB/s");
		//cout << s/t <<',' << maxLatency << ',' << minLatency;
		//cout << ',' << totalLatency/n << endl;
	}
	gb.tft.println("Read");
	for (uint8_t nTest = 0; nTest < READ_COUNT; nTest++) {
		file.rewind();
		maxLatency = 0;
		minLatency = 9999999;
		totalLatency = 0;
		t = millis();
		for (uint32_t i = 0; i < n; i++) {
			buf[BUF_SIZE-1] = 0;
			uint32_t m = micros();
			int32_t nr = file.read(buf, sizeof(buf)); 
			if (nr != sizeof(buf)) {   
				gb.tft.println("READ FAIL");
				while(1);
			}
			m = micros() - m;
			if (maxLatency < m) {
				maxLatency = m;
			}
			if (minLatency > m) {
				minLatency = m;
			}
			totalLatency += m;
			if (buf[BUF_SIZE-1] != '\n') {
				gb.tft.println("ERROR DATA CHECK");
				while(1);
			}
		}
		s = file.fileSize();
		t = millis() - t;
		gb.tft.print("Speed:");
		gb.tft.print(s/t);
		gb.tft.println("KB/s");
//		cout << s/t <<',' << maxLatency << ',' << minLatency;
//		cout << ',' << totalLatency/n << endl;
	}
}

void loop() {
	
}
