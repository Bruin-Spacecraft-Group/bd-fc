#ifndef __AVS_H__
#define __AVS_H__
#include <Arduino.h>
#include "cl.h"
// TODO: clarify these values
#define AVG_HIGH_TRIGGER 16000
#define AVG_LOW_TRIGGER 100
#define XAVG_HIGH_TRIGGER 16000
#define XAVG_LOW_TRIGGER 100
#define XAVG_NEG_TRIGGER 30000


#define LSM9DS0_ADDRESS_XM           (0x1D)         // 3B >> 1 = 7bit default
#define LSM9DS0_ADDRESS_G               (0x6B)         // D6 >> 1 = 7bit default
#define LSM9DS0_VALUE_START_A		(0b10101000)
#define LSM9DS0_VALUE_START_M		(0b10001000)
#define LSM9DS0_VALUE_START_G		(0b10101000)
#define LSM9DS0_REGISTER_WHO_AM_I_G          (0x0F)
#define LSM9DS0_REGISTER_CTRL_REG1_XM        (0x20)
#define LSM9DS0_REGISTER_CTRL_REG2_XM        (0x21)
#define LSM9DS0_REGISTER_CTRL_REG5_XM        (0x24)
#define LSM9DS0_REGISTER_CTRL_REG6_XM        (0x25)
#define LSM9DS0_REGISTER_CTRL_REG7_XM        (0x26)
#define LSM9DS0_REGISTER_WHO_AM_I_XM         (0x0F)
#define LSM9DS0_REGISTER_CTRL_REG1_G         (0x20)
#define LSM9DS0_REGISTER_CTRL_REG4_G         (0x23)
#define LSM9DS0_XM_ID                      (0b01001001)
#define LSM9DS0_G_ID                       (0b11010100)

#define LSM9DS0_REG0_XM (0x1F)
#define LSM9DS0_REG5_G (0x24)
#define LSM9DS0_FIFO_CTRL_REG_G (0x2E)
#define LSM9DS0_FIFO_CTRL_REG_XM (0x2E)

// Linear Acceleration: mg per LSB
#define LSM9DS0_ACCELRANGE_2G  (0b000 << 3)
#define LSM9DS0_ACCELRANGE_4G  (0b001 << 3)
#define LSM9DS0_ACCELRANGE_6G  (0b010 << 3)
#define LSM9DS0_ACCELRANGE_8G  (0b011 << 3)
#define LSM9DS0_ACCELRANGE_16G (0b100 << 3)
#define LSM9DS0_ACCEL_MG_LSB_2G (0.061F)
#define LSM9DS0_ACCEL_MG_LSB_4G (0.122F)
#define LSM9DS0_ACCEL_MG_LSB_6G (0.183F)
#define LSM9DS0_ACCEL_MG_LSB_8G (0.244F)
#define LSM9DS0_ACCEL_MG_LSB_16G (0.732F) // Is this right? Was expecting 0.488F
#define ACCEL_MG_LSB (LSM9DS0_ACCEL_MG_LSB_2G)
#define ACCEL_RANGE (LSM9DS0_ACCELRANGE_2G)

// Magnetic Field Strength: gauss range
#define LSM9DS0_MAGGAIN_2GAUSS (0b00 << 5)  // +/- 2 gauss
#define LSM9DS0_MAGGAIN_4GAUSS (0b01 << 5)  // +/- 4 gauss
#define LSM9DS0_MAGGAIN_8GAUSS (0b10 << 5)  // +/- 8 gauss
#define LSM9DS0_MAGGAIN_12GAUSS (0b11 << 5)   // +/- 12 gauss
#define LSM9DS0_MAG_MGAUSS_2GAUSS      (0.08F)
#define LSM9DS0_MAG_MGAUSS_4GAUSS      (0.16F)
#define LSM9DS0_MAG_MGAUSS_8GAUSS      (0.32F)
#define LSM9DS0_MAG_MGAUSS_12GAUSS     (0.48F)
#define MAG_MGAUSS_LSB     (LSM9DS0_MAG_MGAUSS_2GAUSS)
#define MAG_GAIN (LSM9DS0_MAGGAIN_2GAUSS)

// Angular Rate: dps per LSB
#define LSM9DS0_GYROSCALE_245DPS   (0b00 << 4)  // +/- 245 degrees per second rotation
#define LSM9DS0_GYROSCALE_500DPS   (0b01 << 4)  // +/- 500 degrees per second rotation
#define LSM9DS0_GYROSCALE_2000DPS  (0b10 << 4)  // +/- 2000 degrees per second rotation
#define LSM9DS0_GYRO_DPS_DIGIT_245DPS      (0.00875F)
#define LSM9DS0_GYRO_DPS_DIGIT_500DPS      (0.01750F)
#define LSM9DS0_GYRO_DPS_DIGIT_2000DPS     (0.07000F)
#define GYRO_DPS_DIGIT      (LSM9DS0_GYRO_DPS_DIGIT_245DPS)
#define GYRO_SCALE (LSM9DS0_GYROSCALE_245DPS)

#define SENSORS_GRAVITY_EARTH             (9.80665F)


// Current Sense Stuff
#define INA219_ADDRESS                         (0x40)
#define INA219_READ                            (0x01)
#define INA219_REG_CALIBRATION                 (0x05)
#define INA219_REG_CONFIG                      (0x00)
#define INA219_REG_BUSVOLTAGE                  (0x02)
#define INA219_REG_CURRENT                     (0x04)
#define INA219_REG_POWER                       (0x03)
#define INA219_REG_SHUNTVOLTAGE                (0x01)
#define INA219_CALVALUE_HI		       (0x00)
#define INA219_CALVALUE_LO		       (0x28)
#define INA219_CONFIGVALUE_HI		       (0x9F)
#define INA219_CONFIGVALUE_LO		       (0x39)


// FS2012 Stuff
#define FS2012_ADDRESS  (0x07)

void avsf_init();
void avsf_read(DATA* d);
void av_read(int16_t* V);
void sense_read(int16_t* buf);
void flow_read();

#endif
