byte STATUS; 
/* STATUS BITMASK:
MSB: DEBUG_MODE (pull before flight)



FLIGHT_UP (turn on electrode)
FLIGHT_DOWN (turn off electrode)

LSB:
 */
#define DEBUG_FLAG     0b10000000
#define FLIGHT_UP_FLAG 0b00010000
#define FLIGHT_DN_FLAG 0b00001000

void setup(){
	Serial.begin(9600);
	STATUS |= (pin_check() << 7);
	Serial.println(STATUS);
	Serial.println(DEBUG_FLAG);
	if(STATUS && DEBUG_FLAG){
		debug_mode();
	}
}

void loop(){
	Serial.println("hello there");
	delay(300);
}

void debug_mode(){
	Serial.println("am in debug mode");
	while(true){
		while(Serial.available() == 0)
			;
		char b = Serial.read();
		if(b == 'a'){
			while(STATUS && DEBUG_FLAG){
				STATUS &= (pin_check() << 7);
				Serial.println(STATUS);
				Serial.println(DEBUG_FLAG);
				loop();
				Serial.println("exited loop()");
			}
		}
	}
}

bool pin_check(){
	return(analogRead(A0) == 1023);
}
