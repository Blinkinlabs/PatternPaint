// Pattern Player Sketch
// Designed to work with PatternPaint and the BlinkyTape controller
//
// Tested with the following software:
// Arduino 1.8.13 (https://www.arduino.cc/en/Main/Software)
// FastLED 3.4.0 (https://github.com/FastLED/FastLED/releases/tag/v3.1.3)

#include <FastLED.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "BlinkyTape.h"
#include "SerialLoop.h"
#include "animation.h"

// Pattern table definitions
#define PATTERN_EEPROM_TABLE_ADDRESS (0x100)     // Location of the pattern table in EEPROM memory
#define PATTERN_TABLE_HEADER_LENGTH     11       // Length of the header, in bytes
#define PATTERN_TABLE_ENTRY_LENGTH      9        // Length of each entry, in bytes

// Header data sections
#define PATTERN_COUNT_OFFSET    0    // Number of patterns in the pattern table (1 byte)
#define LED_COUNT_OFFSET        1    // Number of LEDs in the pattern (2 bytes)
#define BRIGHTNESS_OFFSET       3    // Brightness table (8 bytes)

// Entry data sections
#define ENCODING_TYPE_OFFSET    0    // Encoding (1 byte)
#define FRAME_DATA_OFFSET       1    // Memory location (2 bytes)
#define FRAME_COUNT_OFFSET      3    // Frame count (2 bytes)
#define FRAME_DELAY_OFFSET      5    // Frame delay (2 bytes)
#define FRAME_REPEAT_COUNT_OFFSET   7    // Repeat count (2 bytes)


// LED data array
struct CRGB leds[MAX_LEDS];   // Space to hold the current LED data
CLEDController* controller;   // LED controller

// Pattern information
uint8_t patternCount;         // Number of available patterns
uint8_t currentPattern;       // Index of the current patter
Animation pattern;            // Current pattern

uint16_t ledCount;            // Number of LEDs used in the current sketch

const uint8_t brightnessCount = 8;
uint8_t brightnesSteps[brightnessCount];

uint8_t currentBrightness;
uint8_t lastBrightness;

// Button interrupt variables and Interrupt Service Routine
uint8_t buttonState = 0;
bool buttonDebounced;
long buttonDownTime = 0;
long buttonPressTime = 0;

#define BUTTON_BRIGHTNESS_SWITCH_TIME  1     // Time to hold the button down to switch brightness
#define BUTTON_PATTERN_SWITCH_TIME    1000   // Time to hold the button down to switch patterns

#define EEPROM_START_ADDRESS  0
#define PATTERN_EEPROM_ADDRESS EEPROM_START_ADDRESS + 0
#define BRIGHTNESS_EEPROM_ADDRESS EEPROM_START_ADDRESS + 1

// Read the pattern data from the end of the program memory, and construct a new Pattern from it.
void setPattern(uint8_t newPattern) {
  currentPattern = newPattern % patternCount;
  eeprom_write_byte(PATTERN_EEPROM_ADDRESS, newPattern);
  
  const uint16_t patternEntryAddress =
    PATTERN_EEPROM_TABLE_ADDRESS
    + PATTERN_TABLE_HEADER_LENGTH
    + currentPattern * PATTERN_TABLE_ENTRY_LENGTH;

  const Animation::Encoding encodingType = (Animation::Encoding)eeprom_read_byte(patternEntryAddress + ENCODING_TYPE_OFFSET);

  PGM_P frameData =  (PGM_P)eeprom_read_word(patternEntryAddress + FRAME_DATA_OFFSET);
  const uint16_t frameCount = eeprom_read_word(patternEntryAddress + FRAME_COUNT_OFFSET);
  const uint16_t frameDelay = eeprom_read_word(patternEntryAddress + FRAME_DELAY_OFFSET);
  const uint16_t repeatCount = eeprom_read_word(patternEntryAddress + FRAME_REPEAT_COUNT_OFFSET);

  pattern.init(frameCount, frameData, encodingType, ledCount, frameDelay, repeatCount);
}

void setBrightness(uint8_t newBrightness) {
  currentBrightness = newBrightness % brightnessCount;
  eeprom_write_byte(BRIGHTNESS_EEPROM_ADDRESS, currentBrightness);

  LEDS.setBrightness(brightnesSteps[currentBrightness]);
}

// Called when the button is both pressed and released.
ISR(PCINT0_vect) {
  buttonState = !(PINB & (1 << PINB6)); // Reading state of the PB6 (remember that HIGH == released)

  if (buttonState) {
    // On button down, record the time so we can convert this into a gesture later
    buttonDownTime = millis();
    buttonDebounced = false;

    // And configure and start timer4 interrupt.
    TCCR4B = 0x0F; // Slowest prescaler
    TCCR4D = _BV(WGM41) | _BV(WGM40);  // Fast PWM mode
    OCR4C = 0x10;        // some random percentage of the clock
    TCNT4 = 0;  // Reset the counter
    TIMSK4 = _BV(TOV4);  // turn on the interrupt

  }
  else {
    TIMSK4 = 0;  // turn off the interrupt
  }
}

// This is called every xx ms while the button is being held down; it counts down then displays a
// visual cue and changes the pattern.
ISR(TIMER4_OVF_vect) {
  // If the user is still holding down the button after the first cycle, they were serious about it.
  if (buttonDebounced == false) {
    buttonDebounced = true;

    lastBrightness = currentBrightness;
    setBrightness(currentBrightness + 1);
  }

  // If we've waited long enough, switch the pattern
  // TODO: visual indicator
  buttonPressTime = millis() - buttonDownTime;
  if (buttonPressTime > BUTTON_PATTERN_SWITCH_TIME) {
    // first unroll the brightness!
    setBrightness(lastBrightness);

    setPattern(currentPattern + 1);

    // Finally, reset the button down time, so we don't advance again too quickly
    buttonDownTime = millis();
  }
}

void setup()
{
  Serial.begin(57600);

  pinMode(BUTTON_IN, INPUT_PULLUP);

  // Interrupt set-up; see Atmega32u4 datasheet section 11
  PCIFR  |= (1 << PCIF0);  // Just in case, clear interrupt flag
  PCMSK0 |= (1 << PCINT6); // Set interrupt mask to the button pin (PCINT6)
  PCICR  |= (1 << PCIE0);  // Enable interrupt

  // First, load the pattern count and LED geometry from the pattern table
  patternCount = eeprom_read_byte(PATTERN_EEPROM_TABLE_ADDRESS + PATTERN_COUNT_OFFSET);
  ledCount     = eeprom_read_word(PATTERN_EEPROM_TABLE_ADDRESS + LED_COUNT_OFFSET);

  // Next, read the brightness table.
  for (uint8_t i = 0; i < brightnessCount; i++) {
    brightnesSteps[i] = eeprom_read_byte(PATTERN_EEPROM_TABLE_ADDRESS + BRIGHTNESS_OFFSET + i);
  }

  // Bounds check for the LED count
  // Note that if this is out of bounds,the patterns will be displayed incorrectly.
  if (ledCount > MAX_LEDS) {
    ledCount = MAX_LEDS;
  }

  // Read in the last-used pattern and brightness
  currentPattern = eeprom_read_byte(PATTERN_EEPROM_ADDRESS);
  currentBrightness = eeprom_read_byte(BRIGHTNESS_EEPROM_ADDRESS);

  // Now, read the first pattern from the table
  setPattern(currentPattern);
  setBrightness(currentBrightness);

  controller = &(LEDS.addLeds<WS2811, LED_OUT, GRB>(leds, ledCount));
  LEDS.show();
}



void loop()
{
  // If'n we get some data, switch to passthrough mode
  if (Serial.available() > 0) {
    serialLoop(leds);
  }

  pattern.draw(leds);

  if(pattern.getRepeatCount() == 0) {
    setPattern(currentPattern+1);
  }

  // TODO: More sophisticated wait loop to get constant framerate.
  delay(pattern.getFrameDelay());
}
