///////////////////////////////////////////////////////////////////////////////////////////
// Bruin Spacecraft Group
// FLIGHT FLY FLY FLY FLY FLY
//
//     +----------------------------------------------------------+
//    /                   /                    /                / |
//   /                   /                    /                /  |
//  +---------------------------------------------------------+   |
//  |  ______                    _____                        |   |
//  |  | ___ \           ()     /  ___|                       |   |
//  |  | |_/ /_ __ _   _ _ _ __ \ `--. _ __   __  _  ___ ___  |   |
//  |  | ___ \ '__| | | | | '_ \ `--. \ '_ \ / _\| |/ __/ _ \ |   |
//  |  | |_/ / |  | |_| | | | | /\__/ / |_) | (_ | | (__| __/ |   +
//  |  \____/|_|   \__,_|_|_| |_\____/| .__/ \__/|_|\___\___| |  /
//  |                                 | |                     | /
//  |                                 |_|                     |/
//  +------------------------- * * * -------------------------+
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "cl.h"
#include "avsf.h"
#include "nff.h"
#include <avr/wdt.h>

DATA d; // see cl.h for DATA struct

void setup() {
	// prime and disable watchdog
	MCUSR = 0;
	wdt_disable();
	// Set MOSFET as output pin
	pinMode(MOSFET_PIN, OUTPUT);
	// Set baud rate to 115200 (Default serial configureation is 8N1)
	Serial.begin(115200);
	// Set timeout to 20ms (It may take up to 17ms for all of the data to
	// transfer from the NFF, this ensures that enough time has passed
	// to allow for a complete transfer before timing out).
	Serial.setTimeout(20);
	// Initialize avionics board
	avsf_init();
	// Initialize the SD workspace
	cl_sdInit();
	// Check whether A0 & 5V are connected; if yes, drop to debug mode
	cl_setDebugFlag(&d);
	if (bitRead(d.FLAGS, FLAG_DEBUG)) {
		Serial.println(F("Entering Debug Mode"));
		cl_debugMode(&d);
	}
	// reset state to whatever it was before 
	cl_resetState(&d);
}

void loop() {
	// this delay is about 75ms: TODO: try and make it around 90?
	cl_getTime(&d);
	avsf_read(&d);
	nff_getData(&d);
	cl_comb(&d);
	cl_sdWrite(&d);
}
