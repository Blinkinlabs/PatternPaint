// Pattern Player Sketch
// Designed to work with PatternPaint and the BlinkyTape controller
//
// Tested with the following software:
// Arduino 1.8.19 (https://www.arduino.cc/en/Main/Software)
// FastLED 3.7.8 (https://github.com/FastLED/FastLED)

#include <FastLED.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "BlinkyTape.h"
#include "SerialLoop.h"
#include "animation.h"

// LED data array
struct CRGB leds[MAX_LEDS];   // Space to hold the current LED data
CLEDController* controller;   // LED controller

#define BRIGHTNESS_STEPS (8)

// Settings data
uint8_t currentPattern;       // Index of the current pattern
uint8_t currentBrightness;
uint8_t shortPressBehavior;
uint8_t longPressBehavior;
uint8_t patternCount;         // Number of available patterns
uint16_t ledCount;            // Number of LEDs used in the current sketch
uint8_t brightnesSteps[BRIGHTNESS_STEPS]; // Brightness steps

Animation pattern;            // Current pattern

bool singlePatternRepeatMode = false;

// Button interrupt variables and Interrupt Service Routine
uint8_t buttonState = 0;
long buttonDownTime = 0;
long buttonRepeatTime = 0;
long buttonPressTime = 0;




// Read the pattern data from the end of the program memory, and construct a new Pattern from it.
void setPattern(uint8_t newPattern) {
  currentPattern = newPattern % patternCount;
  eeprom_write_byte((uint8_t*)PATTERN_EEPROM_ADDRESS, newPattern);
  
  const uint16_t patternEntryAddress =
    EEPROM_PATTERN_TABLE_ADDRESS 
    + (currentPattern * PATTERN_TABLE_ENTRY_LENGTH);

  const Animation::Encoding encodingType = (Animation::Encoding)eeprom_read_byte((uint8_t*)(patternEntryAddress + ENCODING_TYPE_OFFSET));

  PGM_P frameData =  (PGM_P)eeprom_read_word((uint16_t*)(patternEntryAddress + FRAME_DATA_OFFSET));
  const uint16_t frameCount = eeprom_read_word((uint16_t*)(patternEntryAddress + FRAME_COUNT_OFFSET));
  const uint16_t frameDelay = eeprom_read_word((uint16_t*)(patternEntryAddress + FRAME_DELAY_OFFSET));
  const uint16_t repeatCount = eeprom_read_word((uint16_t*)(patternEntryAddress + FRAME_REPEAT_COUNT_OFFSET));

  pattern.init(frameCount, frameData, encodingType, ledCount, frameDelay, repeatCount);
}

void setBrightness(uint8_t newBrightness) {
  currentBrightness = newBrightness % BRIGHTNESS_STEPS;
  eeprom_write_byte((uint8_t*)BRIGHTNESS_EEPROM_ADDRESS, currentBrightness);

  LEDS.setBrightness(brightnesSteps[currentBrightness]);
}

void handleButtonPress(uint8_t function) {
  if(function == BUTTON_FUNCTION_DISABLED) {
    return;
  }
  else if(function == BUTTON_FUNCTION_BRIGHTNESS) {
    setBrightness(currentBrightness + 1);
  }
  else if(function == BUTTON_FUNCTION_ADVANCE_PATTERN) {
    setPattern(currentPattern + 1);
  }
  else if(function == BUTTON_FUNCTION_TOGGLE_SINGLE_REPEAT) {
    singlePatternRepeatMode = !singlePatternRepeatMode;
  }
}

// Called when the button is both pressed and released.
ISR(PCINT0_vect) {
  buttonState = !(PINB & (1 << PINB6)); // Reading state of the PB6 (remember that HIGH == released)

  if (buttonState) {
    // On button down, record the time so we can convert this into a gesture later
    buttonDownTime = millis();
    buttonRepeatTime = buttonDownTime;

    // And configure and start timer4 interrupt.
    TCCR4B = 0x0F; // Slowest prescaler
    TCCR4D = _BV(WGM41) | _BV(WGM40);  // Fast PWM mode
    OCR4C = 0x10;        // some random percentage of the clock
    TCNT4 = 0;  // Reset the counter
    TIMSK4 = _BV(TOV4);  // turn on the interrupt

  }
  else {
    // On button up, turn off the timer interrupt
    TIMSK4 = 0;

    // And, if the button was only pressed for a short time, handle the short button press.
    buttonPressTime = millis() - buttonDownTime;

    if((buttonPressTime > BUTTON_SHORT_PRESS_TIME) && (buttonPressTime < BUTTON_LONG_PRESS_TIME)) {
      handleButtonPress(shortPressBehavior);
    }
  }
}

// This is called every xx ms while the button is being held down; it counts down then displays a
// visual cue and changes the pattern.
ISR(TIMER4_OVF_vect) {
  // If we've waited long enough, fire a long press
  // TODO: visual indicator
  
  buttonPressTime = millis() - buttonRepeatTime;
  if (buttonPressTime >= BUTTON_LONG_PRESS_TIME) {

    handleButtonPress(longPressBehavior);

    // Finally, reset the button down time, so we don't advance again too quickly
    buttonRepeatTime = millis();
  }
}

void setup()
{
  Serial.begin(57600);

  // Read in the last-used pattern and brightness
  currentPattern = eeprom_read_byte((uint8_t*)PATTERN_EEPROM_ADDRESS);
  currentBrightness = eeprom_read_byte((uint8_t*)BRIGHTNESS_EEPROM_ADDRESS);

  shortPressBehavior = eeprom_read_byte((uint8_t*)BUTTON_SHORT_PRESS_BEHAVIOR_ADDRESS);
  longPressBehavior = eeprom_read_byte((uint8_t*)BUTTON_LONG_PRESS_BEHAVIOR_ADDRESS);

  // First, load the pattern count and LED geometry from the pattern table
  patternCount = eeprom_read_byte((uint8_t*)PATTERN_COUNT_ADDRESS);
  ledCount     = eeprom_read_word((uint16_t*)LED_COUNT_ADDRESS);

  // Bounds check for the LED count
  // Note that if this is out of bounds,the patterns will be displayed incorrectly.
  if (ledCount > MAX_LEDS) {
    ledCount = MAX_LEDS;
  }

  // Next, read the brightness table.
  for (uint8_t i = 0; i < BRIGHTNESS_STEPS; i++) {
    brightnesSteps[i] = eeprom_read_byte((uint8_t*)(BRIGHTNESS_TABLE_ADDRESS + i));
  }

  // Now, read the first pattern from the table
  setPattern(currentPattern);
  setBrightness(currentBrightness);

  controller = &(LEDS.addLeds<WS2811, LED_OUT, GRB>(leds, ledCount));
  LEDS.show();

  // Finally, initialize the button input and interrupts
  pinMode(BUTTON_IN, INPUT_PULLUP);

  // Interrupt set-up; see Atmega32u4 datasheet section 11
  PCIFR  |= (1 << PCIF0);  // Just in case, clear interrupt flag
  PCMSK0 |= (1 << PCINT6); // Set interrupt mask to the button pin (PCINT6)
  PCICR  |= (1 << PCIE0);  // Enable interrupt
}



void loop()
{
  // If'n we get some data, switch to passthrough mode
  if (Serial.available() > 0) {
    serialLoop(leds);
  }

  pattern.draw(leds);

  if(pattern.getDone() && !singlePatternRepeatMode) {
    setPattern(currentPattern+1);
  }

  // TODO: More sophisticated wait loop to get constant framerate.
  delay(pattern.getFrameDelay());
}
