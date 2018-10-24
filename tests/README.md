## Tests folder

Because the way the arduino IDE functions, each sketch will have its own folder (sigh...)

## Writing a new test

- Duplicate the test_template folder
- change the folder name and sketch name to (component)_test
- fill code into loop() as needed
- if additional libraies are needed:
  - in the sketch folder, create a subfolder `lib/` (same level as the sketch)
  - copy entire library folder into `lib/`
  - in code, include using 
  ```
    #include "lib/PATH/TO/HEADER.h"
  ```
  
## Documenting a test

- every test should contain some kind of file, explaining what output is expected given what input and config
