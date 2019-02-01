#include <EEPROM.h>
#include "pff.h"

union{
	unsigned long l;
	byte b[4];
} a;
byte write[512] = {0};
FRESULT fc;
FATFS sd;
UINT bw;

void setup() {
	Serial.begin(9600);
	pf_mount(&sd);
	pf_open("WRITE.TXT");
	a.b[0] = EEPROM.read(0);
	a.b[1] = EEPROM.read(1);
	a.b[2] = EEPROM.read(2);
	a.b[3] = EEPROM.read(3);
	Serial.println(a.l);
}

void loop() {
	pf_write(write, 512, &bw);
	a.l = sd.fptr;
	EEPROM.update(0, a.b[0]);
	EEPROM.update(1, a.b[1]);
	EEPROM.update(2, a.b[2]);
	EEPROM.update(3, a.b[3]);
}
