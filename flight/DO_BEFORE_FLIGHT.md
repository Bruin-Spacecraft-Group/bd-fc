# Pre-shipping software checklist:

1. Compilation environment: hardware buffer needs to be set to 256 bytes:
  look in arduino source code (`HardwareSerial.h`), change `SERIAL_RX_BUFFER_SIZE` to 256

2. SD card file: ensure that sd card has a clean, empty, 15GB+ file
  this is done on macOS or Linux using the `dd` command

3. EEPROM: ensure that entire EEPROM is empty (filled with **0** bytes)! This ensures that the SD card start reading at the beginning of a file

4. Run commands in debug mode, ensure that data is being output and saved correctly

(full procedure to follow)
