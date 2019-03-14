#include "avs.h"

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

void avs_init(){
	// TODO: review this function and the register writes,
	// sensitivity ranges
	Wire.begin();
	uint8_t reg = read8(XMTYPE, LSM9DS0_REGISTER_WHO_AM_I_XM);
	if (reg != LSM9DS0_XM_ID)
		for(;;);

	reg = read8(GYROTYPE, LSM9DS0_REGISTER_WHO_AM_I_G);
	if (reg != LSM9DS0_G_ID)
		for(;;);

	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG1_XM, 0b00110111); // (0b0011 -> 12.5hz XYZ)
	write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG5_XM, 0b01101000);
	// disable temp, 12.5hz mag
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

	// Set up stream mode
  	write8(XMTYPE, LSM9DS0_REG0_XM, 0b01000000);
	write8(XMTYPE, LSM9DSO_FIFO_CTRL_REG_XM, 0b01000000);
  	write8(GYROTYPE, LSM9DS0_REG5_G, 0b01000000);
	write8(GYROTYPE, LSM9DSO_FIFO_CTRL_REG_G, 0b01000000);

	// Set up sense
	write16(INA219_REG_CALIBRATION, INA219_CALVALUE);
	write16(INA219_REG_CONFIG, INA219_CONFIGVALUE);
}

float avs_read(DATA* d){
	float calc;
	// Unwrap and rewrap:
	// dereference d's AV array, and point at which one it wants
	// ==> all are polling at 12.5 hz or once every 0.08 sec;
	for(int av_DATAROW = 0; av_DATAROW < 16; av_DATAROW++){
		av_read((int16_t*) &(d->AV)[av_DATAROW]);
		calc += sqrt(((d->AV)[av_DATAROW][0] ^ 2) + \
				((d->AV)[av_DATAROW][1] ^ 2) + \
				((d->AV)[av_DATAROW][2] ^ 2));
	}
	sense_read((int16_t*) d->SENSE);
	// TODO: check how bad this actually is (timing is everything)
	// TODO: calculate expected and return values (match nff)
	calc /= 16;
	return calc;
}

void sense_read(int16_t* buf){
	uint16_t t;
	t = read16(INA219_REG_SHUNTVOLTAGE);
	buf[0] = (int16_t)t;
	t = read16(INA219_REG_BUSVOLTAGE);
	buf[1] = (int16_t)((t >> 3) * 4);
	write16(INA219_REG_CALIBRATION, INA219_CALVALUE);
	t = read16(INA219_REG_CURRENT);
	buf[2] = (int16_t)(t);
	t = read16(INA219_REG_POWER);
	buf[3] = (int16_t)(t);
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
