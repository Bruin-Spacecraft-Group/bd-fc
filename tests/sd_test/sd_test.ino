// Make printf work

#include <stdarg.h>
void printf(char *fmt, ... ){
        char buf[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        Serial.print(buf);
}


/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/

#include "pff.h"


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
  printf("hello");
}

/*-----------------------------------------------------------------------*/
/* Program Main                                                          */
/*-----------------------------------------------------------------------*/

void loop()
{
  FRESULT rc;
	FATFS fatfs;			/* File system object */
	DIR dir;				/* Directory object */
	FILINFO fno;			/* File information object */
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
	Serial.print("\nOpen a file to write (write.txt).\n");
	rc = pf_open("TEST.TXT");
	if (rc) die(rc);

	Serial.print("\nWrite a text data. (Hello world!)\n");
	for (;;) {
		rc = pf_write("testr", 5, &bw);
		if (rc || !bw) break;
	}
	if (rc) die(rc);

	Serial.print("\nTerminate the file write process.\n");
	rc = pf_write(0, 0, &bw);
	if (rc) die(rc);
#endif

#if _USE_DIR
	Serial.print("\nOpen root directory.\n");
	rc = pf_opendir(&dir, "");
	if (rc) die(rc);

	Serial.print("\nDirectory listing...\n");
	for (;;) {
		rc = pf_readdir(&dir, &fno);	/* Read a directory item */
		if (rc || !fno.fname[0]) break;	/* Error or end of dir */
		if (fno.fattrib & AM_DIR)
			printf("   <dir>  %s\n", fno.fname);
		else
			printf("%8lu  %s\n", fno.fsize, fno.fname);
	}
	if (rc) die(rc);
#endif

	Serial.print("\nTest completed.\n");
	for (;;) ;
}
