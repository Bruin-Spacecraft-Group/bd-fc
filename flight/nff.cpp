#include "nff.h"
#include <Arduino.h>

long get_nff_data(char* buffer) {
	char temp[NFF_MAXFIELDSIZE] = {0};
	int len;
	int l = 0; int i = 1; int p = 0; int shift = 0;
	while(!Serial.available())
		; // hold program until new data
	len = Serial.readBytes(buffer, NFF_MAXBUFSIZE);
	while (l < len) {
	// THIS USES SHORT CIRCUTING AND PREFIX
	// i.e. second comma, next datafield is altitude
		if (buffer[l] == ',' && ++p == NFF_CHECKPOSITION - 1) {
			while(buffer[l+i] != ',') {
				if(!shift && buffer[l+i] == '.') {
					shift = 1;
				}
				else {
					temp[i-1-shift] = buffer[l+i];
				}
				i++;
			}
			return atol(temp);
		}
		l++;
	}
	return ERROR;
}
