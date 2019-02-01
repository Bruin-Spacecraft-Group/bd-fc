#include "pff.h"

/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/


void die (		/* Stop with dying message */
		FRESULT rc	/* FatFs return value */
	 )
{
	Serial.print("Failed with ");
	Serial.println(rc);
	for (;;) ;
}

void setup() {
	// start serial port at 9600 bps:
	Serial.begin(9600);
}

/*-----------------------------------------------------------------------*/
/* Program Main                                                          */
/*-----------------------------------------------------------------------*/

void loop()
{
	FRESULT rc;
	FATFS fatfs;			/* File system object */
	UINT bw, br, i;
	BYTE buff[64];


	Serial.print("\nMount a volume.\n");
	rc = pf_mount(&fatfs);
	if (rc) die(rc);

	Serial.print("\nOpen a test file (message.txt).\n");
	rc = pf_open("TEST.TXT");
	if (rc) die(rc);

	Serial.print("\nType the file content.\n");
	for (;;) {
		rc = pf_read(buff, sizeof(buff), &br);
		if (rc || !br) break;
		for (i = 0; i < br; i++){
			Serial.print(buff[i]);
			Serial.print(" ");
		}
	}
	if (rc) die(rc);

#if _USE_WRITE
	Serial.print("\nOpen a file to write.\n");
	rc = pf_open("TEST.TXT");
	if (rc) die(rc);

	Serial.print("\nWrite a text data\n");
	for (;;) {
		rc = pf_write("abcde ", 6, &bw);
		Serial.println(fatfs.fptr);
		if (rc || !bw) break;
	}
	if (rc) die(rc);

	Serial.print("\nTerminate the file write process.\n");
	rc = pf_write(0, 0, &bw);
	if (rc) die(rc);
#endif

	Serial.print("\nTest completed.\n");
	for (;;) ;
}
