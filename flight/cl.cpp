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
const unsigned long TIMER_TIME = 180000;

void cl_sdInit(){
	uint8_t flags = EEPROM.read(0);
	bool reset = bitRead(flags, FLAG_RESET);
	FRESULT fc = pf_mount(&cl_sdVolume);
	if(fc){
		Serial.print("problem mounting: ");
		Serial.println(fc);
		if(!reset){
			bitWrite(flags, FLAG_RESET, 1);
			EEPROM.write(0, flags);
			wdt_enable(WDTO_500MS);
			for(;;);
		}
	}
	fc = pf_open(SAVE_FILE);
	if(fc){
		Serial.print("problem opening: ");
		Serial.println(fc);
		if(!reset){
			bitWrite(flags, FLAG_RESET, 1);
			EEPROM.write(0, flags);
			wdt_enable(WDTO_500MS);
			for(;;);
		}
	}
}

void cl_sdWrite(DATA* d){
	pf_lseek(d->SD_ADDR);
	FRESULT fc = pf_write((byte*)d, 512, &cl_sdBytesWritten);
	if(fc || cl_sdBytesWritten != 512){
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
	bitWrite(d->FLAGS, FLAG_DEBUG, (analogRead(A0) > 950));
}

void cl_comb(DATA* d){
	unsigned long t = d->time - d->trigger_time;
	if(bitRead(d->FLAGS, FLAG_MOSFET) && t > 30000 && d->SD_ADDR % 5120 == 0 && (d->trigger_time != 0)){
		// eeprom.put(13) will be a uint8 value that goes up to 255: this is n
		// from eeprom.put(14) to eeprom.put(14 + 6n) where n = 167 values
		uint8_t val = EEPROM.read(13);
		if(val < 167){
			EEPROM.put(14 + 6*val, d->FLOW);
			EEPROM.put(16 + 6*val, (d->SENSE)[2]);
			EEPROM.put(18 + 6*val, (d->SENSE)[1]);
		}
		EEPROM.write(13, ++val);
	}
	// expand bits
	int nff = (bitRead(d->FLAGS, FLAG_NFF_H) * 2) + bitRead(d->FLAGS, FLAG_NFF_L);
	int avs = (bitRead(d->FLAGS, FLAG_AVS_H) * 2) + bitRead(d->FLAGS, FLAG_AVS_L);
	// if nothing happened, return
	if(avs == 0 && nff == 0)
		return;
	if(bitRead(d->FLAGS, FLAG_MOSFET)){
		// look for turnoff signals
		if(avs == 3 || nff == 3 || t > TIMER_TIME){
			digitalWrite(MOSFET_PIN, LOW);
			bitWrite(d->FLAGS, FLAG_MOSFET, 0);
		}
		return;
	}
	else{
		// if neither is toggled
		if(avs != 2 && nff != 2){
			// if the timer has not been set, try setting it
			if((avs == 1 || nff == 1) && (d->trigger_time == 0)){
				d->trigger_time = d->time;
				EEPROM.put(9, d->trigger_time);
				return;
			}
			// if timer was set but not there yet, return
			if((d->trigger_time != 0) && t < TIMER_TIME){
				return;
			}
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
	// indicate that it has been reset before
	unsigned long ot, tt;
	EEPROM.get(5, ot);
	EEPROM.get(9, tt);
	unsigned long t = tt - ot;
	if(t > 0){
		// a valid timer was running; take the timer difference and set the trigger time to that
		d->trigger_time = t - TIMER_TIME;
	}
	cl_getTime(d);
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
			case 'e':
				{
					Serial.println(F("EEPROM DUMP++++++"));
					uint8_t f = 0; unsigned long l = 0;
					uint16_t v = 0;
					EEPROM.get(0, f);
					Serial.print(F("EEPROM FLAGS: "));
					for(int i = 0; i < 8; i++){
						Serial.print(bitRead(f, i));
					}
					Serial.println();
					EEPROM.get(1, l);
					Serial.print(F("EEPROM SD: "));
					Serial.println(l);
					EEPROM.get(5, l);
					Serial.print(F("EEPROM TIME: "));
					Serial.println(l);
					EEPROM.get(9, l);
					Serial.print(F("EEPROM TRIGGER TIME: "));
					Serial.println(l);
					Serial.println(F("EEPROM SAVED DATA:"));
					for(int i = 0; i < 167; i++){
						EEPROM.get(14+6*i, v);
						Serial.print(F("flow: "));
						Serial.print(v/10);
						EEPROM.get(16+6*i, v);
						Serial.print(F(" current: "));
						Serial.print((float)v / 25.0);
						EEPROM.get(18+6*i, v);
						Serial.print(F(" voltage: "));
						Serial.print(v * 0.001);
						Serial.println();
					}
					Serial.println(F("EEPROM DUMP END------"));
					break;
				}

			case 'f':
				avsf_read(d);
				Serial.print(F("FLOW (SCCM): "));
				Serial.println((float)d->FLOW/10.0);
				break;
			case 'r':
				Serial.println(F("RESETTING"));
				for(unsigned int i = 0; i < EEPROM.length(); i++){
					EEPROM.put(i, 0);
				}
				memset(d, 0, 512);
				Serial.println(F("DONE"));
				break;

				// Other utilities
			case 't':
				// t command should be appended with full NFF data string
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
				while(analogRead(A0) < 950){
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
				while(analogRead(A0) < 950){
					delay(1000);
					cl_getTime(d);
					avsf_read(d);
					nff_getData(d);
					cl_comb(d);
					Serial.write((byte*)d, 512);
					Serial.println();
				}
				wdt_enable(WDTO_15MS);
				for(;;);
				break;
		}
	}
}
