#ifndef __CORELOGIC_H__
#define __CORELOGIC_H__

// Corelogic (cl): responsible for flight logic, testing harness,
// SD card functions, as well as flowmeter and voltmeter

#include <Arduino.h>

#define FLAG_DEBUG 0b10000000
#define FLAG_UP    0b00010000
#define FLAG_DOWN  0b00001000
#define FLAG_WRITE 0b00000001
#define FLAG_FLOW  0b01000000
// NOTE: THIS IS CASE SENSITIVE
// ALSO PFF DOESN'T LET US USE LOWER CASE
#define SAVE_FILE "WRITE.TXT"

typedef struct{
	unsigned char FLAGS;
	union{
		long l;
		byte b[4];
	}SD_ADDR;
	unsigned long time;
	byte NFF[210];
	int AV[16][9];
	long FLOW;
	unsigned char nul = 0x0;
} DATA;

void cl_setDebugFlag(DATA* d);
void cl_debugMode(DATA d);
void cl_getTime(DATA* d);
void cl_sdInit();
void cl_sdWrite(byte* db);

#endif
