#!/bin/bash

# Stop at any error
set -e

ARDUINO=/Applications/Arduino-1.6.3.app/Contents/MacOS/Arduino
BOARD="blinkinlabs:avr:blinkytape"
HEX_CONVERTER="PatternPaint/PatternPlayer_Sketch/hex_to_header.py"

SKETCH_NAME='PatternPlayer_Sketch'
SKETCH_DIR="$PWD/PatternPaint/PatternPlayer_Sketch"
OUTPUT_DIR="PatternPaint/devices"
OUTPUT_NAME="PATTERNPLAYER"

# Compile the sketch, and extract the .hex filename from the output, then convert it to a header
HEX=`${ARDUINO} --verify ${SKETCH_DIR}/${SKETCH_NAME}.ino --board ${BOARD} --verbose | grep ${SKETCH_NAME}.cpp.hex | sed 's/ *$//g' | grep -oE "[^[:blank:]]+$"`
${HEX_CONVERTER} ${HEX} ${OUTPUT_NAME} > ${OUTPUT_DIR}/${SKETCH_NAME}.h


SKETCH_NAME='ProductionSketch'
SKETCH_DIR="/Users/matt/Blinkinlabs-Projects/BlinkyTape_Arduino/examples/ProductionSketch"
OUTPUT_DIR="PatternPaint/devices"
OUTPUT_NAME="PRODUCTION"

# Compile the sketch, and extract the .hex filename from the output, then convert it to a header
HEX=`${ARDUINO} --verify ${SKETCH_DIR}/${SKETCH_NAME}.ino --board ${BOARD} --verbose | grep ${SKETCH_NAME}.cpp.hex | sed 's/ *$//g' | grep -oE "[^[:blank:]]+$"`
${HEX_CONVERTER} ${HEX} ${OUTPUT_NAME} > ${OUTPUT_DIR}/${SKETCH_NAME}.h
