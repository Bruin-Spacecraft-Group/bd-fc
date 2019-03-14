// Corelogic (cl.cpp): implmentations of SD init/read,
// debug mode, reading from the flowmeter and voltmeter

#include "cl.h"
#include "nff.h"
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
	EEPROM.get(1, d->SD_ADDR);
	pf_lseek(d->SD_ADDR);
	noInterrupts();
	FRESULT fc = pf_write((byte*)d, 512, &cl_sdBytesWritten);
	interrupts();
	if(fc || cl_sdBytesWritten != 512){
		Serial.println(cl_sdBytesWritten);
		Serial.println("problem writing");
		return;
	}
	fc = pf_write(0, 0, &cl_sdBytesWritten);
	d->SD_ADDR = cl_sdVolume.fptr;
	EEPROM.put(1, d->SD_ADDR);
	EEPROM.put(0, d->FLAGS);
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
					Serial.println(avs_read(&d));
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
				break;
			case 'g':
				// av_read returns raw data, for testing use
				// actual data points
				Serial.println(F("\nTesting gyroscope:"));
				for(int i = 0; i < 16; i++){
					Serial.println(avs_read(&d));
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
				break;
			case 'm':
				// av_read returns raw data, for testing use
				// actual data points
				Serial.println(F("\nTesting magnetometer:"));
				for(int i = 0; i < 16; i++){
					Serial.println(avs_read(&d));
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
				break;

			case 'd':
				delay(100);
				cl_getTime(&d);
				Serial.println(avs_read(&d));
				nff_getData(&d);
				Serial.println(F("DUMP:"));
				// dump nff, flowmeter, etc
				Serial.println(d.FLAGS);
				Serial.println(d.time);
				Serial.println(d.SD_ADDR);
				for(int i = 0; i < 204; i++){
					Serial.print(char(d.NFF[i]));
					Serial.print(" ");
				}
				Serial.println();
				for(int i = 0; i < 4; i++){
					Serial.print(d.SENSE[i]);
					Serial.print(" ");
				}
				Serial.println();
				for(int i = 0; i < 16; i++){
					for(int j = 0; j < 9; j++){
						Serial.print(d.AV[i][j]);
						Serial.print(" ");
					}
					Serial.println();
				}
				Serial.println(d.FLOW);
				Serial.println(F("END OF DUMP"));
				break;

			case 't':
				delay(100);
				unsigned long b;
				unsigned long e;
				Serial.print(F("NFF TIME: "));
				b = micros();
				nff_getData(&d);
				e = micros();
				Serial.println(e - b);
				Serial.print(F("AVS TIME: "));
				b = micros();
				avs_read(&d);
				e = micros();
				Serial.println(e - b);
				Serial.print(F("WRITE TIME: "));
				b = micros();
				cl_sdWrite(&d);
				e = micros();
				Serial.println(e - b);
				Serial.println();
				break;

			case 's':
				delay(100);
				Serial.print(F("NFF RETURN GIVEN DATA: "));
				Serial.println(nff_getData(&d));
				break;

			case 'e':
				Serial.println(F("CLEANING EEPROM"));
				for(int i = 0; i < 256; i++)
					EEPROM.put(i, 0);
				break;
			
			case 'w':
				Serial.println(F("WRITING TO SD CARD"));
				Serial.print(F("EEPROM (BEFORE): "));
				unsigned long temp;
				Serial.println(EEPROM.get(1, temp));
				cl_sdWrite(&d);
				Serial.print(F("EEPROM (AFTER): "));
				Serial.println(EEPROM.get(1, temp));
				break;
			case 'p':
				Serial.println(F("TOGGLING MOSFET PIN"));
				digitalWrite(MOSFET_PIN, !digitalRead(MOSFET_PIN));
				break;
			case 'c':
				avs_read(&d);
				Serial.print(F("CURRENT (mA): "));
				Serial.println((float)d.SENSE[2] / 25);
				Serial.print(F("BUS VOLTAGE (V): "));
				Serial.println(d.SENSE[1] * 0.001);
		}
	}
}
