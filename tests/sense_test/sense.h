#ifndef __SENSE_H__
#define __SENSE_H__

#define INA219_ADDRESS                         (0x40)
#define INA219_READ                            (0x01)
#define INA219_REG_CALIBRATION                 (0x05)
#define INA219_REG_CONFIG                      (0x00)
#define INA219_REG_BUSVOLTAGE                  (0x02)
#define INA219_REG_CURRENT                     (0x04)
#define INA219_REG_POWER                       (0x03)
#define INA219_REG_SHUNTVOLTAGE                (0x01)


void sense_init();
void sense_read(int16_t* buf);

#endif
