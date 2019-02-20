#include "aina219.h"
#include <Arduino.h>
#include <Wire.h>

void s_init(){
	Wire.begin();
	setCalibration_32V_1A();
}

void s_read(int16_t* buf){
	//buf: shunt, bus, current, power
	buf[0] = (int16_t)read16(INA219_REG_SHUNTVOLTAGE);
	buf[1] = (int16_t)((read16(INA219_REG_BUSVOLTAGE) >> 3) * 4);
	write16(INA219_REG_CALIBRATION, ina219_calvalue);
	buf[2] = (int16_t)read16(INA_REG_CURRENT);
	buf[3] = (int16_t)read16(INA_REG_POWER);
}

/* note on integrating with av.cpp:
 * override function
 * read8(bool type, byte reg, byte value) with
 * read8(byte addr, byte reg, byte value);
 * this generalizes it to all three addresses
 * (implement bool type version with byte version)
 * on top of this, implement read16(byte reg, byte value)
 */

byte readBuffer(byte addr, byte reg, byte len, uint8_t *buffer){
	Wire.beginTransmission(addr);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(addr, (byte)len);
	// this is here because current sense needs 1 ms to get ready.
	// Hopefully this doesn't cause timing issues
	delay(1);
	for (uint8_t i=0; i<len; i++) {
		buffer[i] = Wire.read();
	}
	return len;
}

byte read8(byte addr, byte reg){
	uint8_t v;
	readBuffer(addr, reg, 1, &v);
	return v;
}

void write8(byte addr, byte reg, byte val){
	Wire.beginTransmission(addr);
	Wire.write(reg);
	Wire.write(val);
	Wire.endTransmission();
}

uint16_t read16(byte reg){
	byte buf[2];
	uint16_t val;
	readBuffer(ina219_i2caddr, reg, 2, buf);
	val = (buf[1]<<8) | buf[0];
	return val;
}

void write16(byte reg, uint16_t val){
	Wire.beginTransmission(ina219_i2caddr);
	Wire.write(reg);
	Wire.write((val>>8)&0xFF); //hi
	Wire.write(val&0xFF); //lo
	Wire.endTransmission();
}

void setCalibration_32V_1A()
{
	// By default we use a pretty huge range for the input voltage,
	// which probably isn't the most appropriate choice for system
	// that don't use a lot of power.  But all of the calculations
	// are shown below if you want to change the settings.  You will
	// also need to change any relevant register settings, such as
	// setting the VBUS_MAX to 16V instead of 32V, etc.
	
	// VBUS_MAX = 32V		(Assumes 32V, can also be set to 16V)
	// VSHUNT_MAX = 0.32	(Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
	// RSHUNT = 0.1			(Resistor value in ohms)
	
	// 1. Determine max possible current
	// MaxPossible_I = VSHUNT_MAX / RSHUNT
	// MaxPossible_I = 3.2A
	
	// 2. Determine max expected current
	// MaxExpected_I = 1.0A
	
	// 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
	// MinimumLSB = MaxExpected_I/32767
	// MinimumLSB = 0.0000305             (30.5�A per bit)
	// MaximumLSB = MaxExpected_I/4096
	// MaximumLSB = 0.000244              (244�A per bit)
	
	// 4. Choose an LSB between the min and max values
	//    (Preferrably a roundish number close to MinLSB)
	// CurrentLSB = 0.0000400 (40�A per bit)
	
	// 5. Compute the calibration register
	// Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
	// Cal = 10240 (0x2800)
	
	ina219_calValue = 10240;
	
	// 6. Calculate the power LSB
	// PowerLSB = 20 * CurrentLSB
	// PowerLSB = 0.0008 (800�W per bit)
	
	// 7. Compute the maximum current and shunt voltage values before overflow
	//
	// Max_Current = Current_LSB * 32767
	// Max_Current = 1.31068A before overflow
	//
	// If Max_Current > Max_Possible_I then
	//    Max_Current_Before_Overflow = MaxPossible_I
	// Else
	//    Max_Current_Before_Overflow = Max_Current
	// End If
	//
	// ... In this case, we're good though since Max_Current is less than MaxPossible_I
	//
	// Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
	// Max_ShuntVoltage = 0.131068V
	//
	// If Max_ShuntVoltage >= VSHUNT_MAX
	//    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
	// Else
	//    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
	// End If
	
	// 8. Compute the Maximum Power
	// MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
	// MaximumPower = 1.31068 * 32V
	// MaximumPower = 41.94176W
	
	// Set multipliers to convert raw current/power values
	ina219_currentDivider_mA = 25;      // Current LSB = 40uA per bit (1000/40 = 25)
	ina219_powerMultiplier_mW = 0.8f;   // Power LSB = 800uW per bit
	
	// Set Calibration register to 'Cal' calculated above
	write16(INA219_REG_CALIBRATION, ina219_calValue);
	
	// Set Config register to take into account the settings above
	uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
	                  INA219_CONFIG_GAIN_8_320MV |
	                  INA219_CONFIG_BADCRES_12BIT |
	                  INA219_CONFIG_SADCRES_12BIT_1S_532US |
	                  INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
	write16(INA219_REG_CONFIG, config);
}
