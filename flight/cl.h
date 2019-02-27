#ifndef __CORELOGIC_H__
#define __CORELOGIC_H__

// Corelogic (cl): responsible for flight logic, testing harness,
// SD card functions, as well as flowmeter and voltmeter

#include <Arduino.h>

#define FLAG_DEBUG 7
#define FLAG_UP    4
#define FLAG_DOWN  3
#define FLAG_WRITE 0
#define FLAG_FLOW  6
#define FLAG_NFF_FAIL 1
// NOTE: THIS IS CASE SENSITIVE
// ALSO PFF DOESN'T LET US USE LOWER CASE
#define SAVE_FILE "WRITE.TXT"

typedef struct{
	unsigned char FLAGS;
	union{
		unsigned long l;
		byte b[4];
	}SD_ADDR;
	unsigned long time;
	byte NFF[204];
	int SENSE[4];
	int AV[16][9];
	int FLOW;
	unsigned char nul = 0x0;
} DATA;

void cl_setDebugFlag(DATA* d);
void cl_debugMode(DATA d);
void cl_getTime(DATA* d);
void cl_sdInit();
void cl_sdWrite(byte* db);

#endif
