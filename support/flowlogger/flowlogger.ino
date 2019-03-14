#include <FreqCounter.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 cur_sense;
long int freq;
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;

void setup(){
	Serial.begin(115200);
	cur_sense.begin();
	cur_sense.setCalibration_32V_1A();
}

void loop(){
	shuntvoltage = cur_sense.getShuntVoltage_mV();
	busvoltage = cur_sense.getBusVoltage_V();
	current_mA = cur_sense.getCurrent_mA();
	power_mW = cur_sense.getPower_mW();
	loadvoltage = busvoltage + (shuntvoltage / 1000);

	FreqCounter::f_comp = 8;
	FreqCounter::start(500);
	while(FreqCounter::f_ready == 0)
		;
	freq = FreqCounter::f_freq;

	Serial.print(micros());
	Serial.print(" Current: ");
	Serial.print(current_mA);
	Serial.print(" Bus Voltage: ");
	Serial.print(busvoltage);
	Serial.print(" Flow Ticks: ");
	Serial.println(freq);
}
