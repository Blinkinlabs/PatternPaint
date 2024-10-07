#ifndef BLINKY_TAPE_H
#define BLINKY_TAPE_H

#include <FastLED.h>

const uint16_t MAX_LEDS = 512;       // Maximum number of LEDs that can be controlled

#define LED_OUT       13
#define BUTTON_IN     10
#define ANALOG_INPUT  A9
#define EXTRA_PIN_A    7
#define EXTRA_PIN_B   11


// The size of the Atmega32u4 EEPROM is 1KB

// General settings start at 0x000
#define EEPROM_SETTINGS_ADDRESS             (0x000)
#define PATTERN_EEPROM_ADDRESS              (EEPROM_SETTINGS_ADDRESS + 0)
#define BRIGHTNESS_EEPROM_ADDRESS           (EEPROM_SETTINGS_ADDRESS + 1)
#define BUTTON_SHORT_PRESS_BEHAVIOR_ADDRESS (EEPROM_SETTINGS_ADDRESS + 2)
#define BUTTON_LONG_PRESS_BEHAVIOR_ADDRESS  (EEPROM_SETTINGS_ADDRESS + 3)

#define PATTERN_COUNT_ADDRESS               (EEPROM_SETTINGS_ADDRESS + 4)  // Number of patterns in the pattern table (1 byte)
#define LED_COUNT_ADDRESS                   (EEPROM_SETTINGS_ADDRESS + 5)  // Number of LEDs in the pattern (2 bytes)
#define BRIGHTNESS_TABLE_ADDRESS            (EEPROM_SETTINGS_ADDRESS + 7)  // Brightness table (8 bytes)

// Pattern table starts at 0x100
// The pattern table is written by PatternPaint, and is not modified by the firmware
// The pattern table is stored in EEPROM to save a (tiny) amount of flash space
#define EEPROM_PATTERN_TABLE_ADDRESS (0x100)     // Location of the pattern table in EEPROM memory
#define PATTERN_TABLE_ENTRY_LENGTH   (9)         // Length of each entry, in bytes


// Entry data sections
#define ENCODING_TYPE_OFFSET      (0)    // Encoding (1 byte)
#define FRAME_DATA_OFFSET         (1)    // Memory location (2 bytes)
#define FRAME_COUNT_OFFSET        (3)    // Frame count (2 bytes)
#define FRAME_DELAY_OFFSET        (5)    // Frame delay (2 bytes)
#define FRAME_REPEAT_COUNT_OFFSET (7)    // Repeat count (2 bytes)

#endif
