volatile int a;

void ir(){
	a++;
}

void setup(){
	Serial.begin(115200);
	digitalWrite(2, HIGH);
	attachInterrupt(digitalPinToInterrupt(2), ir, RISING);
}

void loop(){
	Serial.println(a);
	delay(1000);
}
