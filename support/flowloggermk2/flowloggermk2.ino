#include <Wire.h>

//Adafruit_INA219 cur_sense;
int freq;
int16_t busvoltage = 0;
int16_t current_mA = 0;

void setup(){
	Serial.begin(115200);
	Wire.begin();
  Serial.println("thing");
	//cur_sense.setcalibration_32v_1a();
	Wire.beginTransmission(0x40);
	Wire.write(0x05);
	Wire.write((10240)>>8 & 0xFF);
	Wire.write((10240)    & 0xFF);
	Wire.endTransmission();
	Wire.beginTransmission(0x40);
	Wire.write(0x00);
	Wire.write((14751)>>8 & 0xFF);
	Wire.write((14751)    & 0xFF);
	Wire.endTransmission();
	Serial.println("Ready!");
}

void loop(){
//	shuntvoltage = cur_sense.getShuntVoltage_mV();
//	busvoltage = cur_sense.getBusVoltage_V();
//	current_mA = cur_sense.getCurrent_mA();
//	power_mW = cur_sense.getPower_mW();
//	loadvoltage = busvoltage + (shuntvoltage / 1000);

	Wire.requestFrom(0x07 ,2);
	freq = ((Wire.read() << 8) | Wire.read()) / 10;

	Wire.beginTransmission(0x40);
	Wire.write(0x04);
	Wire.endTransmission();
	delay(1);
	Wire.requestFrom(0x40, 2);
	current_mA = ((Wire.read() << 8) | Wire.read());

	Wire.beginTransmission(0x40);
	Wire.write(0x02);
	Wire.endTransmission();
	delay(1);
	Wire.requestFrom(0x40, 2);
	busvoltage = ((Wire.read() << 8) | Wire.read());
	busvoltage = (busvoltage >> 3) * 4;

	Serial.print(millis());
	Serial.print(" Current: ");
	Serial.print((float)current_mA / 25.0);
	Serial.print(" Bus Voltage: ");
	Serial.print(busvoltage * 0.001);
	Serial.print(" Flow (SCCM): ");
	Serial.println(freq);
	delay(500);
}
