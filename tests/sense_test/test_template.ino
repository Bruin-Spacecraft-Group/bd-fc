/*
*   test_template: Template for component tests
*   
*   How to report success:
*     1. call pulse_success(); blinks on-board LED, sets pin 1 to high for 0.5 seconds
*     2. call Serial.println()
*
*/
void setup() {
  Serial.begin(115200);
  pinMode(1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  
}

void pulse_success() {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(1, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(1, LOW);
}

