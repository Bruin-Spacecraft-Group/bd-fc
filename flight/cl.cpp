// Corelogic (cl.cpp): implmentations of SD init/read,
// debug mode, reading from the flowmeter and voltmeter

#include "cl.h"
#include "nff.h"
#include "avsf.h"
#include "pff.h"
#include <Arduino.h>
#include <avr/wdt.h>
#include <EEPROM.h>

FATFS cl_sdVolume;
UINT cl_sdBytesWritten;

void cl_sdInit(){
	FRESULT fc = pf_mount(&cl_sdVolume);
	if(fc){
		Serial.print("problem mounting: ");
		Serial.println(fc);
		wdt_enable(WDTO_15MS);
		for(;;);
	}
	fc = pf_open(SAVE_FILE);
	if(fc){
		Serial.print("problem opening: ");
		Serial.println(fc);
		wdt_enable(WDTO_15MS);
		for(;;);
	}
}

void cl_sdWrite(DATA* d){
	pf_lseek(d->SD_ADDR);
	FRESULT fc = pf_write((byte*)d, 512, &cl_sdBytesWritten);
	if(fc || cl_sdBytesWritten != 512){
		// TODO: if problem writing, try switching to backup file?
		Serial.print("problem writing: ");
		Serial.println(cl_sdBytesWritten);
		return;
	}
	fc = pf_write(0, 0, &cl_sdBytesWritten);
	d->SD_ADDR = cl_sdVolume.fptr;
	EEPROM.put(1, d->SD_ADDR);
	EEPROM.put(0, d->FLAGS);
}

void cl_setDebugFlag(DATA* d){
	bitWrite(d->FLAGS, FLAG_DEBUG, (analogRead(A0) == 1023));
}

void cl_comb(DATA* d){
	// skip this entire section if it we're done TODO: possibly update?
	if(bitRead(d->FLAGS, FLAG_DONE))
		return;
	// Also update flow flag here
	if(!bitRead(d->FLAGS, FLAG_FLOW))
		bitWrite(d->FLAGS, FLAG_FLOW, !!(d->FLOW));
	unsigned long t = d->time - d->trigger_time;
	// expand bits
	int nff = (bitRead(d->FLAGS, FLAG_NFF_H) * 2) + bitRead(d->FLAGS, FLAG_NFF_L) * 1;
	int avs = (bitRead(d->FLAGS, FLAG_AVS_H) * 2) + bitRead(d->FLAGS, FLAG_AVS_L) * 1;
	if(bitRead(d->FLAGS, FLAG_MOSFET)){
		// look for turnoff signals
		if(avs == 3 || nff == 3 || t > TIMER_TIME){
			digitalWrite(MOSFET_PIN, LOW);
			bitWrite(d->FLAGS, FLAG_DONE, 1);
			bitWrite(d->FLAGS, FLAG_MOSFET, 0);
			return;
		}
	}
	else{
		// if neither is toggled
		if(avs != 2 && nff != 2){
			// if the timer has not been set, try setting it
			if((avs == 1 || nff == 1) && !(d->trigger_time)){
				d->trigger_time = d->time;
				EEPROM.put(9, d->trigger_time);
				return;
			}
			if((avs == 1 || nff == 1))
				return;
			if(avs == 0 && nff == 0)
				return;
			// < can account for negative and zero (TODO: reason it out)
			if(t < TIMER_TIME)
				return;
		}
		// if this code is run, it means either avs/nff is 2 or timer is set and done
		d->trigger_time = d->time;
		EEPROM.put(9, d->trigger_time);
		digitalWrite(MOSFET_PIN, HIGH);
		bitWrite(d->FLAGS, FLAG_MOSFET, 1);
	}	

}

void cl_getTime(DATA* d){
	d->time = millis();
	EEPROM.put(5, d->time);
}

void cl_resetState(DATA* d){
	EEPROM.get(0, d->FLAGS);
	// restore MOSFET pin
	digitalWrite(MOSFET_PIN, bitRead(d->FLAGS, FLAG_MOSFET));
	unsigned long ot, tt;
	EEPROM.get(5, ot);
	EEPROM.get(9, tt);
	unsigned long t = tt - ot;
	if(t > 0){
		// a valid timer was running; take the timer difference and set the trigger time to that
		d->trigger_time = t - TIMER_TIME;
	}
	EEPROM.get(1, d->SD_ADDR);
}

void cl_debugMode(DATA* d){
	while(true){
		if(!Serial.available())
			continue;
		char x = Serial.read();
		switch(x){
			// Avionics board tests
			case 'a':
				Serial.println(F("\nTesting accelerometer:"));
				for(int i = 0; i < 16; i++){
					avsf_read(d);
					cl_getTime(d);
					Serial.print(F("Time (ms): "));
					Serial.print(d->time);
					Serial.print(F(" x: "));
					Serial.print((float)d->AV[i][0]*ACCEL_MG_LSB/1000);
					Serial.print(F(" y: "));
					Serial.print((float)d->AV[i][1]*ACCEL_MG_LSB/1000);
					Serial.print(F(" z: "));
					Serial.println((float)d->AV[i][2]*ACCEL_MG_LSB/1000);
					delay(100);
				}
				break;
			case 'g':
				Serial.println(F("\nTesting gyroscope:"));
				for(int i = 0; i < 16; i++){
					avsf_read(d);
					cl_getTime(d);
					Serial.print(F("Time (ms): "));
					Serial.print(d->time);
					Serial.print(F(" x: "));
					Serial.print((float)d->AV[i][6]*GYRO_DPS_DIGIT/1000);
					Serial.print(F(" y: "));
					Serial.print((float)d->AV[i][7]*GYRO_DPS_DIGIT/1000);
					Serial.print(F(" z: "));
					Serial.println((float)d->AV[i][8]*GYRO_DPS_DIGIT/1000);
					delay(100);
				}
				break;
			case 'm':
				Serial.println(F("\nTesting magnetometer:"));
				for(int i = 0; i < 16; i++){
					avsf_read(d);
					cl_getTime(d);
					Serial.print(F("Time (ms): "));
					Serial.print(d->time);
					Serial.print(F(" x: "));
					Serial.print((float)d->AV[i][3]*MAG_MGAUSS_LSB/1000);
					Serial.print(F(" y: "));
					Serial.print((float)d->AV[i][4]*MAG_MGAUSS_LSB/1000);
					Serial.print(F(" z: "));
					Serial.println((float)d->AV[i][5]*MAG_MGAUSS_LSB/1000);
					delay(100);
				}
				break;
			case 'c':
				avsf_read(d);
				Serial.print(F("CURRENT (mA): "));
				Serial.println((float)d->SENSE[2] / 25);
				Serial.print(F("BUS VOLTAGE (V): "));
				Serial.println(d->SENSE[1] * 0.001);
				break;
			

			// Data aquisition / export
			case 's':
				// Wait 100 ms to load all nff data in serial buffer into dataframe
				delay(100);
				nff_getData(d);
				Serial.println(F("NFF DATA LOADED INTO DATAFRAME"));
				break;
			case 'd':
				// dump all data in the dataframe
				Serial.println(F("DUMP:++++++"));
				// dump nff, flowmeter, etc
				for(int i = 0; i < 7; i++)
					Serial.print(bitRead(d->FLAGS, i));
				Serial.println();
				Serial.println(d->SD_ADDR);
				Serial.println(d->time);
				Serial.println(d->trigger_time);
				for(int i = 0; i < 200; i++){
					Serial.print(char((d->NFF)[i]));
				}
				Serial.println();
				for(int i = 0; i < 4; i++){
					Serial.print(d->SENSE[i]);
					Serial.print(" ");
				}
				Serial.println();
				for(int i = 0; i < 16; i++){
					for(int j = 0; j < 9; j++){
						Serial.print(d->AV[i][j]);
						Serial.print(" ");
					}
					Serial.println();
				}
				Serial.println(d->FLOW);
				Serial.println(F("END OF DUMP------"));
				break;
			case 'f':
				avsf_read(d);
				Serial.print(F("FLOW (SCCM): "));
				Serial.println((float)d->FLOW/10.0);
				break;
			case 'r':
				Serial.println(F("RESETTING"));
				for(int i = 0; i < 256; i++)
					EEPROM.put(i, 0);
				memset(d, 0, 512);
				break;

			// Other utilities
			case 't':
				// t command should be appended with full NFF data string, possibly 2
				delay(100);
				unsigned long b, e;
				Serial.print(F("NFF TIME: "));
				b = micros();
				nff_getData(d);
				e = micros();
				Serial.println(e - b);
				Serial.print(F("AVS TIME: "));
				b = micros();
				avsf_read(d);
				e = micros();
				Serial.println(e - b);
				Serial.print(F("WRITE TIME: "));
				b = micros();
				cl_sdWrite(d);
				e = micros();
				Serial.println(e - b);
				Serial.println();
				break;
			case 'p':
				Serial.println(F("TOGGLING MOSFET PIN"));
				digitalWrite(MOSFET_PIN, !digitalRead(MOSFET_PIN));
				Serial.print(F("MOSFET PIN NOW "));
				Serial.println(digitalRead(MOSFET_PIN) ? "ON" : "OFF");
				bitWrite(d->FLAGS, FLAG_MOSFET, digitalRead(MOSFET_PIN));
				break;
			case 'w':
				Serial.println(F("WRITING TO SD CARD"));
				Serial.print(F("EEPROM (BEFORE): "));
				unsigned long temp;
				Serial.println(EEPROM.get(1, temp));
				cl_sdWrite(d);
				Serial.print(F("EEPROM (AFTER): "));
				Serial.println(EEPROM.get(1, temp));
				break;
			case 'x':
				// Test cl_comb function
				Serial.println(F("COMBING"));
				cl_comb(d);
				Serial.print(F("FLAGS: "));
				for(int i = 0; i < 7; i++)
					Serial.print(bitRead(d->FLAGS, i));
				Serial.println();
				Serial.print(F("TRIGGER TIME: "));
				Serial.println(d->trigger_time);
				break;
			case 'l':
				// Emulate flight loop (resets on end)
				Serial.println(F("FLIGHT LOOP EMULATION"));
				Serial.println(F("PLUG IN A0-5V JUMPER TO EXIT"));
				delay(1000);
				while(analogRead(A0) != 1023){
					cl_getTime(d);
					avsf_read(d);
					nff_getData(d);
					cl_comb(d);
					cl_sdWrite(d);
				}
				wdt_enable(WDTO_15MS);
				for(;;);
				break;
			case 'v':
				// Emulate flight loop (resets on end)
				Serial.println(F("VERBOSE LOOP EMULATION"));
				Serial.println(F("PLUG IN A0-5V JUMPER TO EXIT"));
				delay(1000);
				while(analogRead(A0) != 1023){
					delay(1000);
					cl_getTime(d);
					avsf_read(d);
					nff_getData(d);
					cl_comb(d);
					cl_sdWrite(d);
					for(int i = 0; i < 512; i++)
						Serial.print(((byte*)d)[i]);
				}
				wdt_enable(WDTO_15MS);
				for(;;);
				break;
		}
	}
}
