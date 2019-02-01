/*
 *   Functional_Test: Functional test of sensor array and nff datastream parsing
 *
 *   Every second, this sketch pulls in nff data using their simulator, as well as sensor data from our own board.
 */

#include "pff.h"
#include "av.h"

#define CHECK_POS 3
#define CHECK_VAL 193000000

char buf[200] = {0};
int16_t sensor[9] = {0}; //int16_t is just int16; sensor data as {accel {x, y, z}, mag {x, y, z}, gyro {x, y, z}} 
long val;
FATFS fatfs;
UINT bw;

void setup() {
	Serial.begin(115200);
	Serial.setTimeout(20);
	av_init();
	pf_mount(&fatfs);
	pf_open("WRITE.TXT");
}

void loop() {
	while(!Serial.available())
	; //hold until there is data
	val = nff_data();
	avi_data();
	sd_write();
}

// THIS RETURNS A LONG - its the measurement * 10^-6 (getting rid of floating point)
long nff_data() {
	char temp[20] = {0};
	int len;
	int l = 0; int i = 1; int p = 0; int shift = 0;
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

void avi_data(){
	av_read(sensor);
}

void sd_write(){
  pf_write(sensor, sizeof(sensor), &bw);
	pf_write("|", 1, &bw);
  pf_write(buf, sizeof(buf), &bw);
	pf_write("\n", 1, &bw);
}
