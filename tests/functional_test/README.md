# Blue Dawn Flight Computer Functional Test

The files in this folder support the testing of sensor reading, nff data receiving, and flushing both to the SD card

## Components:

- `buffer`: 200 byte buffer (accommedate nff data entirely) 
- `setup()`:
	- mount sd card, open file, set write to sector boundry
- `nff_data()`: see nff_test
	- read nff provided data from serial using its configuration and store to buffer
	- from buffer, extract the z-acceleration
		- custom parser runs through bytes, looks for commas to figure out what datapoint its looking at (currently looking at altitude at position 3, or *2 commas*)
		- copies the chars into another buffer, removing the decimal point as well
		- atol returns the reading, 10^-6 times smaller
	- relay value to computer (see below)
- `sensor_data()`: see avionics_test
	- read from LSM9DS0 breakout board using I2C, as configured in `av.c` and `av.h`, store in array
	- extract z-acceleration
	- relay value to computer (see below)
- `sd_write()`: see sd_test
	- write nff data from global buffer into sector; append with ' ';
	- write sensor data from global buffer into sector; append with '\n';
	- finalize write(start writing on new sector)

## Special Considerations:

Because in the flight computer is only allowed to read from Serial during flight, this test does not relay nff & sensor z-accel values back via Serial. Instead, an additional library (`SoftwareSerial`) and arduino are required; the two are connected via pins 3(RX) and 4(TX). While the flight computer arduino is connected to a nff-simulator, ideally the support arduino should be connected to a *seperate* computer to relay values to.

## Test setup:

1. Create a suitable file to write to
	- since pff cannot expand file size, we must have a suitable file size from the beginning
	- `dd bs=1k count=X if=/dev/zero of='FILENAME'` works on linux/osx; replace X with filesize (in MB) and FILENAME with all-caps file name (pff only reads DOS filenames). This creates a X MB large files called FILENAME (no `.txt`) that can be written to
2. Setup recieving arduino
	- connect pin 3 on both arduinos, as well as pin 4; connect support arduino to GSE computer
3. Setup nff-simulator emitter
	- on separate computer, launch nff-simulator program (this generates input data as if it were coming from the featherframe)
	- plug in flight computer arduino
