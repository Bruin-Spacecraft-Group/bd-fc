// Corelogic (cl.cpp): implmentations of SD init/read,
// debug mode, reading from the flowmeter and voltmeter

#include "cl.h"
#include "avs.h"
#include "pff.h"
#include <Arduino.h>
#include <EEPROM.h>

FATFS cl_sdVolume;
UINT cl_sdBytesWritten;

// any for(;;) eventually triggers the watchdog and causes a reset

void cl_sdInit(){
	FRESULT fc = pf_mount(&cl_sdVolume);
	if(fc){
		Serial.println("problem mounting");
		for(;;);
	}
	fc = pf_open(SAVE_FILE);
	if(fc){
		Serial.println("problem opening");
		for(;;);
	}
}

void cl_sdWrite(DATA* d){
	unsigned long sd_addr;
	EEPROM.get(1, sd_addr);
	pf_lseek(sd_addr);
	FRESULT fc = pf_write((bytes*)&d, 512, &cl_sdBytesWritten);
	if(fc || cl_sdBytesWritten != 512){
		Serial.println(cl_sdBytesWritten);
		Serial.println("problem writing");
		return;
	}
	fc = pf_write(0, 0, &cl_sdBytesWritten);
	EEPROM.put(1, cl_sdVolume.fptr);
	EEPROM.put(0, d.FLAGS);
	if(fc){
		// why would this happen? is this important?
		for(;;);
	}
}

void cl_setDebugFlag(DATA* d){
	// possible threshold?
	bitWrite(d->FLAGS, FLAG_DEBUG, (analogRead(A0) == 1023));
}

void cl_getTime(DATA* d){
	d->time = millis();
}

// we pass the entire datastructure here because time is not critical
void cl_debugMode(DATA d){
	byte* d_bytes = (byte*)&d;
	while(true){
		if(!Serial.available())
			continue;
		char x = Serial.read();
		switch(x){
			case 'a':
				// av_read returns raw data, for testing use
				// actual data points
				Serial.println(F("\nTesting accelerometer:"));
				for(int i = 0; i < 16; i++){
					avs_read(&d);
					cl_getTime(&d);
					Serial.print(F("Time (ms): "));
					Serial.print(d.time);
					Serial.print(F(" x: "));
					Serial.print((float)d.AV[i][0]*ACCEL_MG_LSB/1000);
					Serial.print(F(" y: "));
					Serial.print((float)d.AV[i][1]*ACCEL_MG_LSB/1000);
					Serial.print(F(" z: "));
					Serial.println((float)d.AV[i][2]*ACCEL_MG_LSB/1000);
					delay(100);
				}
				cl_sdWrite(d_bytes);
				break;
			case 'g':
				// av_read returns raw data, for testing use
				// actual data points
				Serial.println(F("\nTesting gyroscope:"));
				for(int i = 0; i < 16; i++){
					avs_read(&d);
					cl_getTime(&d);
					Serial.print(F("Time (ms): "));
					Serial.print(d.time);
					Serial.print(F(" x: "));
					Serial.print((float)d.AV[i][6]*GYRO_DPS_DIGIT/1000);
					Serial.print(F(" y: "));
					Serial.print((float)d.AV[i][7]*GYRO_DPS_DIGIT/1000);
					Serial.print(F(" z: "));
					Serial.println((float)d.AV[i][8]*GYRO_DPS_DIGIT/1000);
					delay(100);
				}
				cl_sdWrite(d_bytes);
				break;
			case 'm':
				// av_read returns raw data, for testing use
				// actual data points
				Serial.println(F("\nTesting magnetometer:"));
				for(int i = 0; i < 16; i++){
					avs_read(&d);
					cl_getTime(&d);
					Serial.print(F("Time (ms): "));
					Serial.print(d.time);
					Serial.print(F(" x: "));
					Serial.print((float)d.AV[i][3]*MAG_MGAUSS_LSB/1000);
					Serial.print(F(" y: "));
					Serial.print((float)d.AV[i][4]*MAG_MGAUSS_LSB/1000);
					Serial.print(F(" z: "));
					Serial.println((float)d.AV[i][5]*MAG_MGAUSS_LSB/1000);
					delay(100);
				}
				cl_sdWrite(d_bytes);
				break;
			case 'd':
				// dump data really quickly!!!
				Serial.println(F("DUMP:"));
				cl_getTime(&d);
				avs_read(&d);
				// dump nff, flowmeter, etc
				Serial.println(d.time);
				for(int i = 0; i < 16; i++){
					for(int j = 0; j < 9; j++){
						Serial.print(d.AV[i][j]);
						Serial.print(" ");
					}
					Serial.println();
				}
		}
	}
}
