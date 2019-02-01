#include "av.h"

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

bool av_init(){
	Wire.begin();
	uint8_t reg = read8(XMTYPE, LSM9DS0_REGISTER_WHO_AM_I_XM);
	if (reg != LSM9DS0_XM_ID)
		return false;

	reg = read8(GYROTYPE, LSM9DS0_REGISTER_WHO_AM_I_G);
	if (reg != LSM9DS0_G_ID)
		return false;

	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG1_XM, 0b00110111); // 12.5hz XYZ
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG5_XM, 0b01101000); // disable temp, 12.5hz mag
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG7_XM, 0b00000000);
	write8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG1_G, 0b00001111);

	// Set default ranges for the various sensors
	reg = read8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG2_XM);
	reg &= ~(0b00111000); reg |= ACCEL_RANGE;
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG2_XM, reg);

	reg = read8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG6_XM);
	reg &= ~(0b01100000); reg |= MAG_GAIN;
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG6_XM, reg);

	reg = read8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG4_G);
	reg &= ~(0b00110000); reg |= GYRO_SCALE;
	write8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG4_G, reg);

	return true;
}

void av_read(int16_t* A, int16_t* M, int16_t* G){
	uint8_t xlo, ylo, zlo;
	int16_t xhi, yhi, zhi;
	byte buffer[6];

	// Read 6, starting from X LSB
	readBuffer(XMTYPE, LSM9DS0_VALUE_START_A, 6, buffer);

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

	A[0] = xhi;
	A[1] = yhi;
	A[2] = zhi;

	// Read the magnetometer
	readBuffer(XMTYPE, LSM9DS0_VALUE_START_M, 6, buffer);

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
	M[0] = xhi;
	M[1] = yhi;
	M[2] = zhi;

	// Read gyro
	readBuffer(GYROTYPE, LSM9DS0_VALUE_START_G, 6, buffer);

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

	G[0] = xhi;
	G[1] = yhi;
	G[2] = zhi;
}

void av_read(int16_t* V){
	uint8_t xlo, ylo, zlo;
	int16_t xhi, yhi, zhi;
	byte buffer[6];

	// Read 6, starting from X LSB
	readBuffer(XMTYPE, LSM9DS0_VALUE_START_A, 6, buffer);

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

	V[0] = xhi;
	V[1] = yhi;
	V[2] = zhi;

	// Read the magnetometer
	readBuffer(XMTYPE, LSM9DS0_VALUE_START_M, 6, buffer);

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
	V[3] = xhi;
	V[4] = yhi;
	V[5] = zhi;

	// Read gyro
	readBuffer(GYROTYPE, LSM9DS0_VALUE_START_G, 6, buffer);

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

	V[6] = xhi;
	V[7] = yhi;
	V[8] = zhi;
}

