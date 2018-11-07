#include "av.h"

float   _accel_mg_lsb;
float   _mag_mgauss_lsb;
float   _gyro_dps_digit;

byte readBuffer(bool type, byte reg, byte len, uint8_t *buffer){
	byte address;
	if (type == GYROTYPE) {
		address = LSM9DS0_ADDRESS_GYRO;
	} else {
		address = LSM9DS0_ADDRESS_ACCELMAG;
	}
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(address, (byte)len);
	for (uint8_t i=0; i<len; i++) {
		buffer[i] = Wire.read();
	}
	return len;
}

void write8(bool type, byte reg, byte value){
	byte address;
	if (type == GYROTYPE) {
		address = LSM9DS0_ADDRESS_GYRO;
	} else {
		address = LSM9DS0_ADDRESS_ACCELMAG;
	}
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
}

byte read8(bool type, byte reg){
	uint8_t value;
	readBuffer(type, reg, 1, &value);
	return value;
}

void setupAccel(lsm9ds0AccelRange_t range){
	uint8_t reg = read8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG2_XM);
	reg &= ~(0b00111000);
	reg |= range;
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG2_XM, reg );

	switch (range)
	{
		case LSM9DS0_ACCELRANGE_2G:
			_accel_mg_lsb = LSM9DS0_ACCEL_MG_LSB_2G;
			break;
		case LSM9DS0_ACCELRANGE_4G:
			_accel_mg_lsb = LSM9DS0_ACCEL_MG_LSB_4G;
			break;
		case LSM9DS0_ACCELRANGE_6G:
			_accel_mg_lsb = LSM9DS0_ACCEL_MG_LSB_6G;
			break;
		case LSM9DS0_ACCELRANGE_8G:
			_accel_mg_lsb = LSM9DS0_ACCEL_MG_LSB_8G;
			break;    
		case LSM9DS0_ACCELRANGE_16G:
			_accel_mg_lsb =LSM9DS0_ACCEL_MG_LSB_16G;
			break;
	}
}

void setupMag(lsm9ds0MagGain_t gain){
	uint8_t reg = read8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG6_XM);
	reg &= ~(0b01100000);
	reg |= gain;
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG6_XM, reg );
	switch(gain)
	{
		case LSM9DS0_MAGGAIN_2GAUSS:
			_mag_mgauss_lsb = LSM9DS0_MAG_MGAUSS_2GAUSS;
			break;
		case LSM9DS0_MAGGAIN_4GAUSS:
			_mag_mgauss_lsb = LSM9DS0_MAG_MGAUSS_4GAUSS;
			break;
		case LSM9DS0_MAGGAIN_8GAUSS:
			_mag_mgauss_lsb = LSM9DS0_MAG_MGAUSS_8GAUSS;
			break;
		case LSM9DS0_MAGGAIN_12GAUSS:
			_mag_mgauss_lsb = LSM9DS0_MAG_MGAUSS_12GAUSS;
			break;
	}
}

void setupGyro(lsm9ds0GyroScale_t scale){
	uint8_t reg = read8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG4_G);
	reg &= ~(0b00110000);
	reg |= scale;
	write8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG4_G, reg );
	switch(scale)
	{
		case LSM9DS0_GYROSCALE_245DPS:
			_gyro_dps_digit = LSM9DS0_GYRO_DPS_DIGIT_245DPS;
			break;
		case LSM9DS0_GYROSCALE_500DPS:
			_gyro_dps_digit = LSM9DS0_GYRO_DPS_DIGIT_500DPS;
			break;
		case LSM9DS0_GYROSCALE_2000DPS:
			_gyro_dps_digit = LSM9DS0_GYRO_DPS_DIGIT_2000DPS;
			break;
	}
}

bool av_init(){
	Wire.begin();
	uint8_t id = read8(XMTYPE, LSM9DS0_REGISTER_WHO_AM_I_XM);
	if (id != LSM9DS0_XM_ID)
		return false;

	id = read8(GYROTYPE, LSM9DS0_REGISTER_WHO_AM_I_G);
	if (id != LSM9DS0_G_ID)
		return false;

	// Enable the accelerometer continous TODO: is this needed? 0.01 second refresh seems excessive...
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG1_XM, 0x67); // 100hz XYZ
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG5_XM, 0b11110000);
	// enable mag continuous
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG7_XM, 0b00000000);
	// enable gyro continuous
	write8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG1_G, 0x0F); // on XYZ
	// enable the temperature sensor (output rate same as the mag sensor)
	uint8_t tempReg = read8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG5_XM);
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG5_XM, tempReg | (1<<7));


	// Set default ranges for the various sensors  
	setupAccel(LSM9DS0_ACCELRANGE_2G);
	setupMag(LSM9DS0_MAGGAIN_2GAUSS);
	setupGyro(LSM9DS0_GYROSCALE_245DPS);

	return true;
}

void av_read(){
	float x, y, z;
	uint8_t xlo, ylo, zlo;
	int16_t xhi, yhi, zhi;
	// Read the accelerometer
	byte buffer[6];
	readBuffer(XMTYPE, 
			0x80 | LSM9DS0_REGISTER_OUT_X_L_A, 
			6, buffer);

	xlo = buffer[0];
	xhi = buffer[1];
	ylo = buffer[2];
	yhi = buffer[3];
	zlo = buffer[4];
	zhi = buffer[5];

	// Shift values to create properly formed integer (low byte first)
	xhi <<= 8; xhi |= xlo;
	yhi <<= 8; yhi |= ylo;
	zhi <<= 8; zhi |= zlo;

	x = xhi * _accel_mg_lsb  * SENSORS_GRAVITY_EARTH / 1000;
	y = yhi * _accel_mg_lsb  * SENSORS_GRAVITY_EARTH / 1000;
	z = zhi * _accel_mg_lsb  * SENSORS_GRAVITY_EARTH / 1000;
	
	Serial.print("Accel Data: ");
	Serial.print("X: ");Serial.print(x);
	Serial.print(" Y: ");Serial.print(y);
	Serial.print(" Z: ");Serial.println(z);


	// Read the magnetometer
	readBuffer(XMTYPE, 
			0x80 | LSM9DS0_REGISTER_OUT_X_L_M, 
			6, buffer);

	xlo = buffer[0];
	xhi = buffer[1];
	ylo = buffer[2];
	yhi = buffer[3];
	zlo = buffer[4];
	zhi = buffer[5];

	// Shift values to create properly formed integer (low byte first)
	xhi <<= 8; xhi |= xlo;
	yhi <<= 8; yhi |= ylo;
	zhi <<= 8; zhi |= zlo;
	x = xhi * _mag_mgauss_lsb / 1000;
	y = yhi * _mag_mgauss_lsb / 1000;
	z = zhi * _mag_mgauss_lsb / 1000;
	
	Serial.print("Mag Data: ");
	Serial.print("X: ");Serial.print(x);
	Serial.print(" Y: ");Serial.print(y);
	Serial.print(" Z: ");Serial.println(z);

	// Read gyro
	readBuffer(GYROTYPE, 
			0x80 | LSM9DS0_REGISTER_OUT_X_L_G, 
			6, buffer);

	xlo = buffer[0];
	xhi = buffer[1];
	ylo = buffer[2];
	yhi = buffer[3];
	zlo = buffer[4];
	zhi = buffer[5];

	// Shift values to create properly formed integer (low byte first)
	xhi <<= 8; xhi |= xlo;
	yhi <<= 8; yhi |= ylo;
	zhi <<= 8; zhi |= zlo;

	x = xhi * _gyro_dps_digit / 1000;
	y = yhi * _gyro_dps_digit / 1000;
	z = zhi * _gyro_dps_digit / 1000;
	
	Serial.print("Gyro Data: ");
	Serial.print("X: ");Serial.print(x);
	Serial.print(" Y: ");Serial.print(y);
	Serial.print(" Z: ");Serial.println(z);
  Serial.println();
}

