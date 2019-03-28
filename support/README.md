# Blue Dawn Support Software

### `support/extractor`:

a simple tool to extract data from the SD card, in raw form. After a serial connection is set up to arduino flashed with this software, it will dump all content (including unused space) in the SD card

### `support/flowlogger`:

used for MHD pump / flowmeter tests. Outputs as follows:

board time | current | bus voltage | flow
--- | --- | --- | --- 
milliseconds | milliamps | volts | ticks

This is outputted every 10 milliseconds or so.

Flowmeter ticks convert to LPM via this equation: (`ticks` * 60) / 22000) = `Liters per minute`

### `support/interruptor`:

used for testing how the flight computer handles flowmeter input, by emulating pulses from the flowmeter. This sketch toggles digital pin 2 around every 100 milliseconds.
If fc implements flowmeter data capture correctly, when interruptor arduino pin 2 is connected to fc pin 2, the fc should report 5 pulses / second. (10 toggles / second = 5 rising edges)

### `support/logger`:

used for automatially displaying and saving data from test runs. Mostly will be used with `support/extractor` (for dumping out all data from the SD card) as well as `support/flowlogger` (for logging data when running pump tests).
