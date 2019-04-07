#ifndef __CORELOGIC_H__
#define __CORELOGIC_H__

// Corelogic (cl): responsible for flight logic, testing harness,
// SD card functions, as well as flowmeter and voltmeter
#include <Arduino.h>

#define FLAG_DEBUG 7
// RESET flag: if turned on, don't attempt another reset
#define FLAG_RESET 6
// Mosfet on/off
#define FLAG_MOSFET 5
/* FLAG_NFF_H | FLAG_NFF_L | Status
 * 1|1|H return: turn off mosfet
 * 1|0|F return: turn on mosfet/unset timer
 * 0|1|C return: set timer
 * 0|0|No return found yet (RETURN TO STATE)
 */
#define FLAG_NFF_H 4
#define FLAG_NFF_L 3
/* FLAG_AVS_H | FLAG_AVS_L | Status
 * 1|1|neg_high_x: turn off mosfet
 * 1|0|zero-x: turn on mosfet/unset timer
 * 0|1|high_x: set timer
 * 0|0|No return found yet (RETURN TO STATE)
 */
#define FLAG_AVS_H 2
#define FLAG_AVS_L 1
// Flow on/off
#define FLAG_FLOW 0

// NOTE: THIS IS CASE SENSITIVE
// ALSO PFF DOESN'T LET US USE LOWER CASE
#define SAVE_FILE "WRITE.TXT"
#define MOSFET_PIN 4

typedef struct{
	unsigned char FLAGS = 0; // 1 byte
	unsigned long SD_ADDR; // 4 bytes
	unsigned long time = 0;
	unsigned long trigger_time = 0;
	byte NFF[200]; // 200 bytes
	int SENSE[4]; // 8 bytes
	int AV[16][9]; // 288 bytes
	int FLOW; // 2 bytes
	unsigned char nul = 0x20; // 1 byte
} DATA;

void cl_comb(DATA* d);
void cl_setDebugFlag(DATA* d);
void cl_debugMode(DATA* d);
void cl_getTime(DATA* d);
void cl_sdInit();
void cl_sdWrite(DATA* d);
void cl_resetState(DATA* d);

#endif
