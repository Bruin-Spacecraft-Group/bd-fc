# Testing Blue Dawn Flight Software

## Pretest Checklist (Hardware):
- SD card is plugged in
	- if not, Serial communication will complain  and continuously reset itself
- SD card contains a file 
	- if not, Serial will complain  and continuously reset
- Jumper cable connected between A0 and 5V pins (might be changed as current sense occupies 5V)
	- drops fc into debug mode

## Pretest Checklist (Software):
- Was the buffer size increased before compiling??? increase the hardware buffer size
- FLAG & TIMER are reset in EEPROM
	- by default, the fc restores the previous state after a power cycle (as to not overwrite the data we already have during flight). However, if dropping into debug mode, this state recovery does not happen.

## Dataframe Description
- `unsigned char FLAGS`
- 8 bits to store flags: from LSB to MSB:
- FLOW, (AVS_L|AVS_H), (NFF_L|NFF_H), MOSFET, DONE, DEBUG
	- FLOW: single flag that is only toggled when flow is detected
	- AVS_L/H: 2-bit vector to encode 4 states of avs data
	- NFF_L/H: 2-bit vector to encode 4 states of nff data
	- MOSFET: dictates whether the MOSFET should be on or not
	- DONE: 'stop' flag; when set, set/unsetting other flags no longer have any effect.
	- DEBUG: used to detect whether fc is currently in debug mode
- `unsigned long SD_ADDR`
	- address of datatype used in SD library. Used directly to store/load current pointer to/from EEPROM. 
- `unsigned long time`
	- holds the current time as returned by micros(). updated at the beginning of every loop
- `unsigned long trigger_time`
	- represents where the timer is. It can either be MOSFET on timer, which is set when launch is detected, or MOSFET off timer, which is set after the mosfet is turned on.
	- is represented as a micros() value that the current time is compared against
- `byte NFF[200]`
	- represents the current recieved input from NFF. bytes are just unsigned chars.
- `int SENSE[4]`
	- outputs 4 values, in this order:
	- SHUNT_VOLTAGE (V)
	- BUS_VOLTAGE (V)
	- CURRENT (mA)
	- POWER (mW)
- `int AV[16][9]`
	- outputs 9 data fields 16 times: accel(x/y/z), gyro(x/y/z), mag(x/y/z)
- `int FLOW`
	- outputs flow (in the crappy way we had flow before)
- `nul = 0x0`
	- single byte to help us determine frames post-flight

## Command Documentation

All commands in debug mode are single characters. All but two return after completing their task.

- `a`: accelerometer testing
	- fills dataframe with avionics data, and prints the time (ms) + acceleration x/y/z of each in gees of acceleration (1 gee = 9.8m/s^2). Does not write any of the data to SD card.
- `g`: gyroscope testing
	- fills dataframe with avionics data, and prints the time (ms)+ rotation x/y/z of each in units of 245 degrees per second. Does not write any of the data to SD card.
- `m`: magnetometer testing
	- fills dataframe with avionics data, and prints the time (ms) + magnetic field strength x/y/z of each in units of 2 gauss. Does not write any of the data to SD card.
- `c`: current sensor testing
	- reads single set of avionics data into dataframe, and prints the time (ms) + current (mA) + bus voltage (V) across the current sensor (this is intended to be the flow of electricity across the electrodes.
- `s`: load string into nff
	- loads a string supplied into the dataframe, setting nff flags as expected. This function has an implicit delay of 100ms; its intended use is ('s'+'[full nff string]'), and the delay ensures that the full nff string may be contained within buffer.
- `d`: dump current dataframe
	- dumps out entire dataframe in the order of the dataframe. flags are in binary from LSB to MSB. Everything else is in raw form.
- `f`: print flow
	- This function is likely to be changed. Right now, it flushes however many pulses were in the dataframe before, waits a full second, and returns the # of pulses counted within that second.
- `r`: reset
	- This function may need some tweaking. Right now, it clears out everything in the EEPROM (by setting everything to zero) and also clears out the entire dataframe.
- `t`: time functions
	- this function attempts to time the serial buffer flush, avionics read, and sd write functions.
	- good tests would be to pull out certain parts and see whether it hangs here. For typical performance, t should be appended with a full NFF string. 
- `p`: toggle mosfet pin
	- directly toggle mosfet pin.
- `w`: write dataframe to SD card
	- other then `t`, this is the only command that actually writes data to the SD card. 
- `x`: test logic
	- while nff_getData & avs_read both set the flags indicated by the data they collect,  actually performs the actions the flags suggest.
- `l`: emulate flight loop
	- This function puts the flight computer into a state that emulates the flight loop as closely as possible. It is expected that the A05V jumper is disconnected before invoking this function; due to flight loop not having an exit, plugging in the jumper indicates that the flight computer should exit the loop and reset itself. 
- `v`: verbose emulate flight loop
	- similar to `l`, this function emulates flight loop. It runs every 1100 ms instead of 100 ms, and outputs the entire dataframe via serial in byte form.
