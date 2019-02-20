#include "nff.h"
#include "cl.h"
#include <Arduino.h>

// TODO: integrate this with our new data-structure

long nff_getData(DATA* d) {
	char temp[NFF_MAXFIELDSIZE] = {0};
	int len;
	int l = 0; int i = 1; int p = 0; int shift = 0;
	len = Serial.readBytes(d->NFF, NFF_MAXBUFSIZE);
	if(len == 0){
		// this would be the case where Serial times out, and reads 0 bytes
		// TODO: check what readBytes actually does when setTimeout
		// is configured; does it just return 0?
		// set some kind of flag here to let cl know not getting nff
	}
	while (l < len) {
	// THIS USES SHORT CIRCUTING AND PREFIX
	// i.e. second comma, next datafield is altitude
		if (d->NFF[l] == ',' && ++p == NFF_CHECKPOSITION - 1) {
			while(d->NFF[l+i] != ',') {
				if(!shift && d->NFF[l+i] == '.') {
					shift = 1;
				}
				else {
					temp[i-1-shift] = d->NFF[l+i];
				}
				i++;
			}
			return atol(temp);
		}
		l++;
	}
	return ERROR;
}
