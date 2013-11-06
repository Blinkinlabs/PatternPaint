This sketch is the basis for the program that PatternPaint uploads to the BlinkyTape.

To convert the sketch into a .h file for PatternPaint:
1. Open the sketch in Arduino 1.0.5
2. In Arduino Preferences, check that 'Show verbose output during: compilation' is selected.
3. Press Command+R to compile the sketch
4. Scroll down to the bottom of the output window; the second to last line should show the location of a .hex file. Mine looks something like this:
/var/folders/0d/6pr0k02913z3b7w9pm8gbc180000gn/T/build4201988184645967020.tmp/PatternPlayer_Sketch.cpp.hex
5. Run the included Python sketch to convert the hex file into a c++ data header:

6. Copy this new header into the Pattern Paint program, compile it, and enjoy!


These are the steps that happen when you click upload in pattern paint:
1. Pattern Paint compresses the current animation into an RGB565 color space, and then further compresses that data using RLE.
2. Pattern Paint creates a hex image to flash to the BlinkyTape, by appending the data from that animation to the data from this sketch.
3. Pattern Paint uploads this combined image onto the BlinkyTape, effectively reprogramming it.
4. Pattern Paint sends a reset command to the bootloader and disconnects from it, cauisng the uploaded program to run, and the animation to display.
