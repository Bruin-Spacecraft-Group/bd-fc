/*
 *   sense_test: testing voltage senseor
 *
 */
void setup() {
	Serial.begin(115200);
	sense_init();
	int16_t buf[4];
}

void loop() {
	// from start to end: shunt (mv), bus (V), current (mA), power (mW)
	sense_read(buf);
	Serial.print((float)buf[1]);
	Serial.print(", ");
	Serial.print((float)buf[2]);
	delay(250);
}
