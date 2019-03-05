#define SD_FILE "WRITE.TXT"
#include <SD.h>

typedef struct{
	unsigned char FLAGS;
	unsigned long SD_ADDR;
	unsigned long time;
	byte NFF[204];
	int SENSE[4];
	int AV[16][9];
	int FLOW;
	unsigned char nul = 0x0;
} DATA;

File f;
DATA d;
byte* db = (byte*)&d;

void setup(){
  Serial.begin(115200);
	SD.begin(8);
	f = SD.open(SD_FILE, FILE_READ);
}

void loop(){
	while(f.available()){
		f.read(db, 512);
		Serial.println("+++++");
		Serial.print("FLAGS: ");
		Serial.println(d.FLAGS);
		Serial.print("Time: ");
		Serial.println(d.time);
		Serial.print("NFF DATA: ");
		for(int i = 0; i < 204; i++){
			Serial.print(char(d.NFF[i]));
		}
		Serial.println();
		Serial.print("SENSE DATA: ");
		for(int i = 0; i < 4; i++){
			Serial.print(d.SENSE[i]);
			Serial.print(" ");
		}
		Serial.println();
		Serial.print("AV DATA: ");
		for(int i = 0; i < 16; i++){
			for(int j = 0; j < 9; j++){
				Serial.print(d.AV[i][j]);
				Serial.print(" ");
			}
			Serial.println();
		}
		Serial.print("FLOW: ");
		Serial.println(d.FLOW);
		Serial.println("-----");
	}
}
