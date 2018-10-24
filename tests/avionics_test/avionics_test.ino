/*
*   avionics_test: testing the adafruit 9DOF board
*   
*   wiring: at this point, the sensor board should be mounted on our shield and
*   can be directed put upon the arduino. It should be using I2C on pins A4 (SDA), A5(SCL)
*   to read/write from the sensor.
*   
*/

#include "lib/Adafruit_9DOF/Adafruit_9DOF.h"
#include "lib/Adafruit_LSM303DLHC/Adafruit_LSM303.h"
#include "lib/Adafruit_L3GD20/Adafruit_L3GD20.h"
#include "lib/Adafruit_Sensor/Adafruit_Sensor.h"
#include <Wire.h>

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified();
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified();
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified();

void setup() {
  Serial.begin(115200);
  pinMode(1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  accel.begin(); mag.begin(); gyro.begin();
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);
  Serial.print(F("ACCEL "));
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  
  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  mag.getEvent(&event);
  Serial.print(F("MAG   "));
  Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");
  gyro.getEvent(&event);
  Serial.print(F("GYRO  "));
  Serial.print("X: "); Serial.print(event.gyro.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.gyro.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.gyro.z); Serial.print("  ");Serial.println("rad/s ");

  Serial.println(F(""));
  pulse_success();
}

void pulse_success() {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(1, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(1, LOW);
}

