#include "nff.h"
#include "cl.h"
#include <Arduino.h>

/* Return types:
 * 0: nothing
 * 1: microgravity imminent
 * 2: microgravity ended (shut down most features)
 */
int nff_getData(DATA* d) {
	if(!Serial.available())
		return 0;
	int len = Serial.readBytes(d->NFF, 200);
	int l = 0;
	while (l < len) {
		if (d->NFF[l] == 'C')
			return 1;
		if (d->NFF[l] == 'F')
			return 2;
		l++;
	}
	return 0;
}
