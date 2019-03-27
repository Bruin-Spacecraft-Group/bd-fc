#include "nff.h"
#include "cl.h"
#include <Arduino.h>

void nff_getData(DATA* d) {
	if(!Serial.available())
		return 0;
	int len = Serial.readBytes(d->NFF, 200);
	int l = 0;
	while (l < len) {
		if (d->NFF[l] == 'F'){
			bitWrite(d->FLAGS, FLAG_NFF_H, 1);
			bitWrite(d->FLAGS, FLAG_NFF_L, 0);
			return;
		}
		if (d->NFF[l] == 'H'){
			bitWrite(d->FLAGS, FLAG_NFF_H, 1);
			bitWrite(d->FLAGS, FLAG_NFF_L, 1);
			return;
		}
		if (d->NFF[l] == 'C'){
			bitWrite(d->FLAGS, FLAG_NFF_H, 0);
			bitWrite(d->FLAGS, FLAG_NFF_L, 1);
			return;
		}
		l++;
	}
	bitWrite(d->FLAGS, FLAG_NFF_H, 0);
	bitWrite(d->FLAGS, FLAG_NFF_L, 0);
	return;
}

