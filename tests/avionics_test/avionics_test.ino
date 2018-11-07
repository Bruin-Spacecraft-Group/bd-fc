/*
*   avionics_test: testing the adafruit 9DOF board
*   
*   wiring: at this point, the sensor board should be mounted on our shield and
*   can be directed put upon the arduino. It should be using I2C on pins A4 (SDA), A5(SCL)
*   to read/write from the sensor.
*   
*/

#include "av.h"

void setup() {
  Serial.begin(115200);
  pinMode(1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  av_init();
}

void loop() {
  av_read();
  pulse_success();
}

void pulse_success() {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(1, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(1, LOW);
}

