#!/bin/bash

# Firmware builder for BlinkyTape devices in PatternPaint
#
# To use this, you'll need to install Arduino (1.8.1 is current as of writing).
#
# You'll also need to add the following board support packages:
# * Blinkinlabs32u4 boards
#
# And the following libraries:
# * FastLED (available through the library manager)
# * BlinkyTape (download from https://github.com/Blinkinlabs/BlinkyTape_Arduino/releases)
#
# Note for Windows: you'll also need MinGW to run this script, as described in the PatternPaint readme.



# Stop at any error
set -e

if [ -z ${ARDUINO+x} ]; then
	echo "ARDUINO not defined- please set it to the full path to the Arduino executable. For example:"
	echo "macOS: export ARDUINO=/Applications/Arduino-1.6.12.app/Contents/MacOS/Arduino"
	echo "Windows: export ARDUINO=/c/Program\ Files\ \(x86\)/Arduino/arduino.exe"
	exit 1
fi

BOARD="blinkinlabs:avr:blinkytape"
ARDUINO_FLAGS="--verify --verbose --preserve-temp-files --board ${BOARD}"

# Project root
BASEDIR=`pwd`

# Directory to place the compiled firmware
OUTPUT_DIR=${BASEDIR}/src/libblinky/firmware/blinkytape

# Temporary location to store firmware repos
FIRMWARE=${BASEDIR}/firmware

# BlinkyTape default firmware
BLINKYTAPE_FIRMWARE=${FIRMWARE}/BlinkyTape

################## Get device firmware repositories ##############
function getRepo {
	# $1 is output directory
	# $2 is repo location

	if [ ! -d "$1" ]; then
		git clone --depth 1 $2 $1
	else
		pushd $1
		git pull
		popd
	fi
}

getRepo ${BLINKYTAPE_FIRMWARE} https://github.com/Blinkinlabs/BlinkyTape_Arduino.git


################## Compile the firmware ##########################
function compileFirmware {
	# $1 is the base sketch directory
	# $2 is the sketch name
	# $3 is the hex file output directory
	# $4 is the hex filename

	# Compile the sketch, and extract the .hex filename from the output, then convert it to a header
	HEX=`"${ARDUINO}" ${ARDUINO_FLAGS} ${1}/${2}/${2}.ino \
	 | grep ${2}\.ino\.hex \
	 | sed 's/\"//g' \
	 | grep -oE "[^[:blank:]]+$"`
	echo ${HEX}

	mv "${HEX}" ${3}/${4}/${4}.hex
}

compileFirmware ${BASEDIR} "PatternPlayer_Sketch" ${OUTPUT_DIR} default
compileFirmware ${BLINKYTAPE_FIRMWARE}/examples "ProductionSketch" ${OUTPUT_DIR} factory
