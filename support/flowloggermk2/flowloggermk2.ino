#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 cur_sense;
int freq;
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;

void setup(){
	Serial.begin(115200);
	cur_sense.setCalibration_32V_1A();
	Wire.begin();
	Serial.println("Ready!");
}

void loop(){
	shuntvoltage = cur_sense.getShuntVoltage_mV();
	busvoltage = cur_sense.getBusVoltage_V();
	current_mA = cur_sense.getCurrent_mA();
	power_mW = cur_sense.getPower_mW();
	loadvoltage = busvoltage + (shuntvoltage / 1000);

	Wire.beginTransmission(0x0F);	
	Wire.write(0);
	Wire.beginTransmission(0x0F);	

	Wire.requestFrom(0x0F ,2);
	if (Wire.available())
		freq = ((Wire.read() << 8) | Wire.read()) / 10;

	Serial.print(millis());
	Serial.print(" Current: ");
	Serial.print(current_mA);
	Serial.print(" Bus Voltage: ");
	Serial.print(busvoltage);
	Serial.print(" Flow (SCCM): ");
	Serial.println(freq);
	delay(500);
}
