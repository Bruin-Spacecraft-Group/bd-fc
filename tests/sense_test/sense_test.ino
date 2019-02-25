/*
 *   sense_test: testing voltage senseor
 *
 */

#include "sense.h"
extern uint32_t ina219_currentDivider_mA;
int16_t buf[4];

void setup() {
	Serial.begin(115200);
	sense_init();
}

void loop() {
	// from start to end: shunt (mv), bus (V), current (mA), power (mW)
	sense_read(buf);
	Serial.print((float)buf[1] * 0.001);
	Serial.print(", ");
	Serial.print((float)buf[2] / ina219_currentDivider_mA);
	delay(250);
}
