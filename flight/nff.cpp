#include "nff.h"
#include <Arduino.h>

// TODO: integrate this with our new data-structure

long get_nff_data(char* buffer) {
	char temp[NFF_MAXFIELDSIZE] = {0};
	int len;
	int l = 0; int i = 1; int p = 0; int shift = 0;
	len = Serial.readBytes(buffer, NFF_MAXBUFSIZE);
	if(len == 0){
		// this would be the case where Serial times out, and reads 0 bytes
		// TODO: check what readBytes actually does when setTimeout
		// is configured; does it just return 0?
		// set some kind of flag here to let cl know not getting nff
	}
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
