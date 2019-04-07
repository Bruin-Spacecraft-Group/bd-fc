#define SD_FILE "WRITE.TXT"
#include <SD.h>

typedef struct{
	unsigned char FLAGS;
	unsigned long SD_ADDR;
	unsigned long time;
	unsigned long trigger_time;
	byte NFF[200];
	int SENSE[4];
	int AV[16][9];
	int FLOW;
	unsigned char nul = 0x20;
} DATA;

File f;
unsigned long pos = 0;
DATA d;
byte* db = (byte*)&d;

void setup(){
	Serial.begin(115200);
	SD.begin(8);
	f = SD.open(SD_FILE, FILE_READ);
}

void loop(){
	while(f.available()){
		if(Serial.read() == 'n'){
			f.read(db, 512);
			pos = f.position();
			Serial.print("+++++ Current Position: ");
			Serial.println(pos);
			Serial.print("FLAGS: ");
			for(int i = 0; i < 8; i++)
				Serial.print(bitRead(d.FLAGS, i)); 
			Serial.println();
			Serial.print("SD ADDR: ");
			Serial.println(d.SD_ADDR);
			Serial.print("Time: ");
			Serial.println(d.time);
			Serial.print("Trigger Time: ");
			Serial.println(d.trigger_time);
			Serial.print("NFF DATA: ");
			for(int i = 0; i < 200; i++){
				Serial.print(char(d.NFF[i]));
			}
			Serial.println();
			Serial.print("Current: ");
			Serial.println((float)d.SENSE[2] / 25.0);
			Serial.println("Voltage: ");
			Serial.println((float)d.SENSE[1] * 0.001);
			Serial.println("AV DATA: ");
			for(int i = 0; i < 16; i++){
				Serial.print(F("ax: "));
				Serial.print((float)d.AV[i][0]*0.061F/1000);
				Serial.print(F(" ay: "));
				Serial.print((float)d.AV[i][1]*0.061F/1000);
				Serial.print(F(" az: "));
				Serial.print((float)d.AV[i][2]*0.061F/1000);
				Serial.print(F(" gx: "));
				Serial.print((float)d.AV[i][6]*0.00875F/1000);
				Serial.print(F(" gy: "));
				Serial.print((float)d.AV[i][7]*0.00875F/1000);
				Serial.print(F(" gz: "));
				Serial.print((float)d.AV[i][8]*0.00875F/1000);
				Serial.print(F(" mx: "));
				Serial.print((float)d.AV[i][3]*0.08F/1000);
				Serial.print(F(" my: "));
				Serial.print((float)d.AV[i][4]*0.08F/1000);
				Serial.print(F(" mz: "));
				Serial.print((float)d.AV[i][5]*0.08F/1000);
				Serial.println();
			}
			Serial.print("FLOW: ");
			Serial.println(d.FLOW/10);
			Serial.println("-----");
		}
		if(Serial.read() == 's'){
			delay(100);
			long p = Serial.parseInt();
			pos = f.position();
			pos += p;
			f.seek(pos);
			Serial.print("Seeked to position: ");
			Serial.println(pos);
		}
	}
}
