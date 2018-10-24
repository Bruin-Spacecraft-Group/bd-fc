# bd-fc: Bluedawn Arduino Flight Computer

## Hardware Used

- Flight Config:
  - Arduino uno rev 3, ATmega328P
  - USB 2.0 Type B connection, provided by NFF
  - Power provided: 5V, 0.9A
- Additional For Test Config:
  - Voltage Umbilical
  
## Components

- `start()`:
  - depending on EEPROM: recover past data or
  - initalize and perform checks: is everything connected? is data coming through?
- `loop()`:
  - the main loop that controls the processes of all sub-components
- `NFF Parser nff_prase()`:
  - cleanly brings in nff data, parses it into relevant data fields
  - returns data in array
- `Avionics Parser avionics_parse()`:
  - MEMORY HEAVY
  - cleanly reads avionics board data, parses it into relevant data fields
  - returns data in array
- `flowmeter_parse()`:
  - cleanly reads flowmeter data
  - may block further execution?
  - returns single data field
- `combine(nffArr, aviArr)`:
  - combines NFF return data and avionics data into a single array
  - core logic depends on a single variable (has microgravity?) 
    - don't waste memory space
- `write(telemArr, flow)`:
  - this should be allowed as much time as possible; watchdog should reset right beforehand
  - dual purpose: write to SD, write to EEPROM
    - write full csv telemArr and flow to SD card
    - write flight-specific data to EEPROM
      - (in case of watchdog reset, recovery mechanism)
      - possible: `long` is 4 bytes, write entire flowmeter data to EEPROM?
- `electrode(bool)`:
  - this should not be its own function: less functions = less stack jumping = more execution
  - turns the electrode on/off
- `watchdog`:
  - this should not be a function: watchdog runs seperately, resets arduino if killswitch is not renewed
  
## Guidelines:

- Avoid `float` if possible: avr has no native support for floating point
- Avoid unnecessary function calls
- Use static memory: declare large variables global/static:
  - for example, nff data should always go into a specific memory space: don't put in stack or heap!
- put constant strings in flash memory! (this shouldn't be much of a problem)

## Repo Organization:
`tests/`: code for testing various subcomponents
`support/`: code for new GSE telemetryProcessors, as well as code to flash on supporting devices (umbilical)
`flight/`: flight code~
`ftests/`: test suite to push specific nff and avionics datasets to the FC, detailing its reponses
