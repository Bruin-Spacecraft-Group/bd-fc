#include <Wire.h>
#include "sense.h"
#include <Arduino.h>

uint32_t ina219_calValue = 10240;
uint32_t ina219_currentDivider_mA = 15;
float    ina219_powerMultiplier_mW = 0.8f;
uint16_t ina219_configValue = 14751;

void write16(uint8_t reg, uint16_t val){
	Wire.beginTransmission(INA219_ADDRESS);
	Wire.write(reg);
	Wire.write((val >> 8) & 0xFF); Wire.write(val & 0xFF);
	Wire.endTransmission();
}

uint16_t read16(uint8_t reg){
	Wire.beginTransmission(INA219_ADDRESS);
	Wire.write(reg);
  Wire.endTransmission();
	uint16_t val;
	delay(1);
	Wire.requestFrom(INA219_ADDRESS, 2);
	val = ((Wire.read() << 8) | Wire.read());
	return val;
}

void sense_init(){
	Wire.begin();
	write16(INA219_REG_CALIBRATION, ina219_calValue);
	write16(INA219_REG_CONFIG, ina219_configValue);
}

void sense_read(int16_t* buf){
	uint16_t t;
	t = read16(INA219_REG_SHUNTVOLTAGE);
	buf[0] = (int16_t)t;
	t = read16(INA219_REG_BUSVOLTAGE);
	buf[1] = (int16_t)((t >> 3) * 4);
	write16(INA219_REG_CALIBRATION, ina219_calValue);
	t = read16(INA219_REG_CURRENT);
	buf[2] = (int16_t)(t);
	t = read16(INA219_REG_POWER);
	buf[3] = (int16_t)(t);
}
	
