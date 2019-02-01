// Corelogic (cl.cpp): implmentations of SD init/read,
// debug mode, reading from the flowmeter and voltmeter

#include "cl.h"
#include "av.h"
#include "pff.h"
#include <Arduino.h>

FATFS cl_sdVolume;
UINT cl_sdBytesWritten;

void cl_sdInit(){
	FRESULT fc = pf_mount(&cl_sdVolume);
	if(!fc){
		// if it doesn't work, what should the FC do?
	}
	fc = pf_open(SAVE_FILE);
	if(!fc){
		// what if we can't open the savefile?
	}
}

void cl_sdWrite(byte* db){
	// TODO: this also needs to update eeprom accordingly
	FRESULT fc = pf_write(db, 512, &cl_sdBytesWritten);
	if(!fc || cl_sdBytesWritten != 512){
		// what should we do if saving didn't work?
	}
	fc = pf_write(0, 0, &cl_sdBytesWritten);
	if(!fc){
		// why would this happen? is this important?
	}
}

void cl_setDebugFlag(DATA* d){
	bitWrite(d->FLAGS, 7, (analogRead(A0) == 1023));
}

void cl_getTime(DATA* d){
	d->time = millis();
}

// we pass the entire datastructure here because time is not critical
void cl_debugMode(DATA d){
	byte* d_bytes = (bytes*)&d;
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
					av_read(&d);
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
					av_read(&d);
					cl_getTime(&d);
					Serial.print(F("Time (ms): "));
					Serial.print(d.time);
					Serial.print(F(" x: "));
					Serial.print((float)d.AV[i][0]*GYRO_DPS_DIGIT/1000);
					Serial.print(F(" y: "));
					Serial.print((float)d.AV[i][1]*GYRO_DPS_DIGIT/1000);
					Serial.print(F(" z: "));
					Serial.println((float)d.AV[i][2]*GYRO_DPS_DIGIT/1000);
					delay(100);
				}
				cl_sdWrite(d_bytes);
				break;
			case 'm':
				// av_read returns raw data, for testing use
				// actual data points
				Serial.println(F("\nTesting magnetometer:"));
				for(int i = 0; i < 16; i++){
					av_read(&d);
					cl_getTime(&d);
					Serial.print(F("Time (ms): "));
					Serial.print(d.time);
					Serial.print(F(" x: "));
					Serial.print((float)d.AV[i][0]*MAG_MGAUSS_LSB/1000);
					Serial.print(F(" y: "));
					Serial.print((float)d.AV[i][1]*MAG_MGAUSS_LSB/1000);
					Serial.print(F(" z: "));
					Serial.println((float)d.AV[i][2]*MAG_MGAUSS_LSB/1000);
					delay(100);
				}
				cl_sdWrite(d_bytes);
				break;
		}
	}
}
