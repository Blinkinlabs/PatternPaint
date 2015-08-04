# This sketch is the basis for the program that PatternPaint uploads to the BlinkyTape.

To convert the sketch into a .h file for PatternPaint:

Note: These instructions are for OS X 10.10, please adjust the paths to your OS

1. Install Arduino 1.6.3 into the system applications directory
2. Download and install the Blinkinlabs32u hardware extensions for Arduino (link?)
3. Download and install FastLED version ??
3. Open a terminal session and cd to the PatternPaint directory:

	cd path_to_patternpaint_repo
	ARDUINO = /Applications/Arduino-1.6.3.app/Contents/MacOS/Arduino
	SKETCH = $PWD/PatternPaint/PatternPlayer_Sketch/PatternPlayer_Sketch.ino
	BOARD = blinkinlabs:avr:blinkytape
	
	${ARDUINO} --verify ${SHETCH} --board ${BOARD} --verbose
	
	/Applications/Arduino-1.6.3.app/Contents/MacOS/Arduino --verify $PWD/PatternPaint/PatternPlayer_Sketch/PatternPlayer_Sketch.ino --board blinkinlabs:avr:blinkytape --verbose > result.txt
	cat result.txt |grep 'PatternPlayer_Sketch.cpp.hex' | sed 's/ *$//g' | grep -oE "[^[:blank:]]+$"
	
	

5. Run the included Python sketch to convert the hex file into a c++ data header:
./hex_to_header.py /var/folders/0d/6pr0k02913z3b7w9pm8gbc180000gn/T/build4984830816021265745.tmp/PatternPlayer_Sketch.cpp.hex > ../PatternPlayer_Sketch.h


Note: we expect the output of compiling the program with Arduino to produce the following output:

	(stuff)
	/Users/matt/Library/Arduino15/packages/arduino/tools/avr-gcc/4.8.1-arduino2/bin/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 /var/folders/g4/3zx07nm93cn1l5v3mg_3z2fw0000gn/T/build1459876006165833523.tmp/PatternPlayer_Sketch.cpp.elf /var/folders/g4/3zx07nm93cn1l5v3mg_3z2fw0000gn/T/build1459876006165833523.tmp/PatternPlayer_Sketch.cpp.eep 
	/Users/matt/Library/Arduino15/packages/arduino/tools/avr-gcc/4.8.1-arduino2/bin/avr-objcopy -O ihex -R .eeprom /var/folders/g4/3zx07nm93cn1l5v3mg_3z2fw0000gn/T/build1459876006165833523.tmp/PatternPlayer_Sketch.cpp.elf /var/folders/g4/3zx07nm93cn1l5v3mg_3z2fw0000gn/T/build1459876006165833523.tmp/PatternPlayer_Sketch.cpp.hex 

	Sketch uses 8,388 bytes (29%) of program storage space. Maximum is 28,672 bytes.
	Global variables use 986 bytes (38%) of dynamic memory, leaving 1,574 bytes for local variables. Maximum is 2,560 bytes.

The above expression extracts the '/var/folders/g4/3zx07nm93cn1l5v3mg_3z2fw0000gn/T/build1459876006165833523.tmp/PatternPlayer_Sketch.cpp.hex' part


These are the steps that happen when you click upload in pattern paint:
1. Pattern Paint compresses the current pattern into an RGB565 color space, and then further compresses that data using RLE.
2. Pattern Paint creates a hex image to flash to the BlinkyTape, by appending the data from that pattern to the data from this sketch.
3. Pattern Paint uploads this combined image onto the BlinkyTape, effectively reprogramming it.
4. Pattern Paint sends a reset command to the bootloader and disconnects from it, cauisng the uploaded program to run, and the pattern to display.
