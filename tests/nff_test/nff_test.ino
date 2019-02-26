/*
 * nff_tests: this sketch checks position 3 of the data for a datapoint
 * ~193000000 * 10^-6 ft/m^2 (data is extracted in long). this would be
 * equivalent to getting ~6G of acceleration (note direction is unspecified)
 *
 */
#define CHECK_POS 13
#define CHECK_VAL 193000000

long val;
char buf[200] = {0};

void setup() {
	Serial.begin(115200);
	Serial.setTimeout(20);
	pinMode(LED_BUILTIN, OUTPUT);
}

long get_nff_data() {
  delay(1000);
	char temp[20] = {0};
	int len;
	int l = 0; int i = 1; int p = 0; int shift = 0;
	while(!Serial.available())
		; // hold program until new data
	len = Serial.readBytes(buf, 200);
	while (l < len) {
		// THIS USES SHORT CIRCUTING AND PREFIX i.e. second comma, next datafield is altitude
		if (buf[l] == ',' && ++p == CHECK_POS - 1) {
			while(buf[l+i] != ',') {
				if(!shift && buf[l+i] == '.') {
					shift = 1;
				}
				else {
					temp[i-1-shift] = buf[l+i];
				}
				i++;
			}
			return atol(temp);
		}
		l++;
	}
	return 0;
}

void loop() {
	val = get_nff_data();
	Serial.println(val);
	if (val && val > CHECK_VAL){
		Serial.print("Check Reached!");
		digitalWrite(LED_BUILTIN, HIGH);
	}
}
