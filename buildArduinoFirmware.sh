#!/bin/bash

# Stop at any error
set -e

# Note: Use Arduino 1.6.12
ARDUINO="/Applications/Arduino-1.6.12.app/Contents/MacOS/Arduino"
BOARD="Blinkinlabs:avr:blinkytape"
HEX_CONVERTER="src/PatternPlayer_Sketch/hex_to_header.py"

SKETCH_NAME='PatternPlayer_Sketch'
SKETCH_DIR="$PWD/src/PatternPlayer_Sketch"
OUTPUT_DIR="src/libblinky/controllers"
OUTPUT_NAME="PATTERNPLAYER"

ARDUINO_FLAGS="--verify --verbose --preserve-temp-files --board ${BOARD}"

# Compile the sketch, and extract the .hex filename from the output, then convert it to a header
#
# | grep ${SKETCH_NAME}\.ino\.hex \     search for a line that contains the .hex filename
HEX=`${ARDUINO} ${ARDUINO_FLAGS} ${SKETCH_DIR}/${SKETCH_NAME}.ino \
 | grep ${SKETCH_NAME}\.ino\.hex \
 | sed 's/\"//g' \
 | grep -oE "[^[:blank:]]+$"`

${HEX_CONVERTER} ${HEX} ${OUTPUT_NAME} > ${OUTPUT_DIR}/${SKETCH_NAME}.h


SKETCH_NAME='ProductionSketch'
SKETCH_DIR="$PWD/..//BlinkyTape_Arduino/examples/ProductionSketch"
OUTPUT_DIR="src/controllers"
OUTPUT_NAME="PRODUCTION"

# Compile the sketch, and extract the .hex filename from the output, then convert it to a header

HEX=`${ARDUINO} ${ARDUINO_FLAGS} ${SKETCH_DIR}/${SKETCH_NAME}.ino \
 | grep ${SKETCH_NAME}\.ino\.hex \
 | sed 's/\"//g' \
 | grep -oE "[^[:blank:]]+$"`

${HEX_CONVERTER} ${HEX} ${OUTPUT_NAME} > ${OUTPUT_DIR}/${SKETCH_NAME}.h
