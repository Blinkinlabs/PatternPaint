# This sketch is the basis for the program that PatternPaint uploads to the BlinkyTape.

These are the steps that happen when you click upload in pattern paint:
1. Pattern Paint compresses the current pattern into an RGB565 color space, and then further compresses that data using RLE.
2. Pattern Paint creates a hex image to flash to the BlinkyTape, by appending the data from that pattern to the data from this sketch.
3. Pattern Paint uploads this combined image onto the BlinkyTape, effectively reprogramming it.
4. Pattern Paint sends a reset command to the bootloader and disconnects from it, cauisng the uploaded program to run, and the pattern to display.
