# SD Card Tests

This sketch is a test for a smaller SD library for the Arduino.

## Recreating:
- Using the Petit FatFs (pff) library found [here](http://elm-chan.org/fsw/ff/00index_p.html)
- Download the examples, copy the `generic` folder contents into an Arduino sketchbook
- `main.c` => `SD_test.ino`, added a bit a [code](https://playground.arduino.cc/Main/Printf) to make printf work
- in `pffconfig.h`, set `_USE_WRITE` to `1`, and allow for `FAT_32`
- implement `mmcbbp.c` (see below)

## Setup:

Using the [sparkfun SD card shield](https://www.sparkfun.com/products/12761), we have the following configuration:

Function | MicroSD Pinout | Arduino
----- | ---- | ----
Chip Select | CS | D8
SPI Clock | SCK or CLK | D13
Data Input | DI | D11 (MOSI)
Data Output | DO | D12 (MISO)

MOSI: Master out slave in, i.e. output from arduino and into SD card 
MISO: master in slave out, i.e. reverse

Notice CS, SCK, and DI pins must be set as OUTPUT pins, and SO is an input pin. The sparkfun shield is wired so that these pins pass through a HEX converter, which shifts 5V output into a microSD compatible 3.3V. **HOWEVER**, the DO pin (D12 on arduino) is not pass through the hex converter; the arduino will recieve 3.3V input on Pin 12.

## Software implementation:

[Quick Reference] Input: `CS <=> D8`, `SCK <=> D13`, `DI <=> D11`, Output: `DO <=> D12`

**if your project uses different pins, make sure to read this**

Also read this about bitmath: https://www.peterbeard.co/blog/post/why-is-arduino-digitalwrite-so-slow/

The generic example implements the disk I/O layer (abstracted in pff) in `mmcbbp.c`. Much of the code has been written, implementing writing to a disk via bit-banging. All that is left to implement is the bit-banging methods themselves!

- `include <Arduino.h>`
	- for fast digital pin setting (see below), as well as micro-second delay
- `INIT_PORT()`:
	- set CS/CLK/DI to output, DO to input
	- `DDRB` is a bit vector that holds the I/O states of pins 8-13 (1 = HIGH, 0 = LOW)
	- `DDRB` = `B00101001` (8 to far left, first two bits on right not used)
	- equivalent to `pinMode(8, HIGH)....` but takes less time
- `DLY_US(n)`:
	- delay for n microseconds
	- Arduino provides (delayMicroseconds(unsigned int))[https://www.arduino.cc/en/pmwiki.php?n=Reference/DelayMicroseconds] that's accurate from 3us to 16000us)
	- code only calls `DLY_US(1000)` and `DLY_US(100)`, so fine to use
- `FORWARD(d)`:
	- not sure what this function does; has something to do with processing bits as they come in, which is useless for this project; set to `;` (nop)
- `(CS/CK/DI)_(H/L)`: set pin to high/low:
	- easiest way to do this is by bitmathing `PORTB`
	- `PORTB` contains the output states of pins 8-13 (a la `DDRB`)
	- `PORTB |= B00000001` sets pin 8 to HIGH
		- x OR 1 = 1, x OR 0 = x, changes pin 8 only
	- `PORTB &= B11111110` sets pin 8 to LOW
		- x OR 0 = 0, x OR 1 = x, changes pin 8 only
- `DO`: read DO pin
	- `PINB` contains all the current values of pins 8-13
	- to get pin 12: shift right by 4 (make 12 Least Significant Bit), then AND with 1
	- x & 1 = x; gets rid of other parts of the bit vector, returns either 0 or 1

## Issues

- We don't know if it works at all yet
- Because `INIT_PORT()` sets `DDRB`, any other component that uses pins 8-13 will have their state lost (think freqCounter).


## Acknoledgements
Petit FatFs Module Source Files R0.03                (C)ChaN, 2014


FILES

  pff.h      Common include file for Petit FatFs and application module.
  pff.c      Petit FatFs module.
  diskio.h   Common include file for Petit FatFs and disk I/O module.
  diskio.c   Skeleton of low level disk I/O module.
  integer.h  Alternative type definitions for integer variables.

  Low level disk I/O module is not included in this archive because the Petit
  FatFs module is only a generic file system layer and not depend on any
  specific storage device. You have to provide a low level disk I/O module that
  written to control your storage device.



AGREEMENTS

 Petit FatFs module is an open source software to implement FAT file system to
 small embedded systems. This is a free software and is opened for education,
 research and commercial developments under license policy of following trems.

  Copyright (C) 2014, ChaN, all right reserved.

 * The Petit FatFs module is a free software and there is NO WARRANTY.
 * No restriction on use. You can use, modify and redistribute it for
   personal, non-profit or commercial use UNDER YOUR RESPONSIBILITY.
 * Redistributions of source code must retain the above copyright notice.
