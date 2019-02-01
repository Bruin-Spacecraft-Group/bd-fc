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
#include "av.h"
#include "nff.h"

DATA d; // this line explains everything about this project; see cl.h for DATA struct
byte* d_bytes = (byte*)&d; //d_bytes allows us to access the d struct as if it were an array of bytes

long nff_datapoint = 0;
FATFS sd_volume;
UINT sd_wlen;

void setup(){
	// TODO: testing mode (watchdog disable, reuse functions, verbose)
	Serial.begin(115200);     // Set baud rate to 115200 (Default serial configureation is 8N1).
	// Set timeout to 20ms (It may take up to 17ms for all of the data to
	// transfer from the NFF, this ensures that enough time has passed
	// to allow for a complete transfer before timing out).
	Serial.setTimeout(20);
	// Initialize avionics board
	av_init();
	// Mount and open text file to write to
	// TODO: make a better, more fault-proof version of this (requires watchdog)
	// (ensure its openable, check return flags for problems)
	pf_mount(&sd_volume);
	pf_open("WRITE.TXT");
	// Check whether A0 & 5V are connected; if yes, drop to debug mode
	cl_setDebugFlag(&d);
	// NOTE: CHANGE THIS WHEN DONE WITH DEBUG MODE
	if(true /*& d.FLAGS & FLAG_DEBUG*/){
		Serial.println(F("Entering Debug Mode"));
		cl_debugMode(d);
	}
}

void loop(){
	// This will hold until nff gives us data
	//nff_datapoint = get_nff_data(nff_buf);
	// Read avionics data
	// TODO: add return codes to these, so fc can figure out whether to write or not
	Serial.println("hello");
	//sd_write();
}
