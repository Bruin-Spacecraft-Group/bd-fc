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
#include "avs.h"
#include "nff.h"

DATA d; // see cl.h for DATA struct

long nff_datapoint = 0;

void setup() {
  // Set baud rate to 115200 (Default serial configureation is 8N1)
  Serial.begin(115200);
  // Set timeout to 20ms (It may take up to 17ms for all of the data to
  // transfer from the NFF, this ensures that enough time has passed
  // to allow for a complete transfer before timing out).
  Serial.setTimeout(20);
  // Initialize avionics board
  avs_init();
  // Initialize the SD workspace
  cl_sdInit();
  // Check whether A0 & 5V are connected; if yes, drop to debug mode
  cl_setDebugFlag(&d);
  // TODO: remove always debug
  if (1 || bitRead(d.FLAGS, FLAG_DEBUG)) {
    Serial.println(F("Entering Debug Mode"));
    cl_debugMode(d);
  }
}

void loop() {
  cl_getTime(&d);
  // blocking? watchdog? terminate after time?
  avs_read(&d);
  while (!Serial.available())
    ;
  nff_datapoint = nff_getData(&d);
  Serial.println(nff_datapoint);
  cl_sdWrite(&d);
  delay(90);
}
