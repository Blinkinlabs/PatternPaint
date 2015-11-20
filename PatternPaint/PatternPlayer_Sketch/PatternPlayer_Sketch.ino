#include <animation.h>

#include <FastLED.h>
#include <avr/pgmspace.h>

#include "BlinkyTape.h"
#include "SerialLoop.h"

// Pattern table definitions
#define PATTERN_TABLE_ADDRESS  (0x7000 - 0x80)   // Location of the pattern table in the flash memory
#define PATTERN_TABLE_HEADER_LENGTH     3        // Length of the header, in bytes
#define PATTERN_TABLE_ENTRY_LENGTH      7        // Length of each entry, in bytes

// Header data sections
#define PATTERN_COUNT_OFFSET    0    // Number of patterns in the pattern table (1 byte)
#define LED_COUNT_OFFSET        1    // Number of LEDs in the pattern (2 bytes)

// Entry data sections
#define ENCODING_TYPE_OFFSET    0    // Encoding (1 byte)
#define FRAME_DATA_OFFSET       1    // Memory location (2 bytes)
#define FRAME_COUNT_OFFSET      3    // Frame count (2 bytes)
#define FRAME_DELAY_OFFSET      5    // Frame delay (2 bytes)


// LED data array
struct CRGB leds[LED_COUNT];   // Space to hold the pattern
uint16_t ledCount;             // Actual number of LEDs present

// Pattern information
volatile uint8_t patternCount;         // Number of available patterns
volatile uint8_t currentPattern;         // Index of the current patter
Animation pattern;            // Current pattern
int frameDelay = 30;          // Number of ms each frame should be displayed.

#define BRIGHT_STEP_COUNT 8
#define STARTING_BRIGHTNESS 4
volatile uint8_t brightnesSteps[BRIGHT_STEP_COUNT] = {5,15,40,70,93, 70, 40, 15};

uint8_t brightness = STARTING_BRIGHTNESS;
uint8_t lastBrightness = STARTING_BRIGHTNESS;

// Button interrupt variables and Interrupt Service Routine
uint8_t buttonState = 0;
bool buttonDebounced;
long buttonDownTime = 0;
long buttonPressTime = 0;

#define BUTTON_BRIGHTNESS_SWITCH_TIME  1     // Time to hold the button down to switch brightness
#define BUTTON_PATTERN_SWITCH_TIME    1000   // Time to hold the button down to switch patterns

// Read the pattern data from the end of the program memory, and construct a new Pattern from it.
void loadPattern(uint8_t newPattern) {
  currentPattern = newPattern;
  
  uint16_t patternEntryAddress =
        PATTERN_TABLE_ADDRESS
        + PATTERN_TABLE_HEADER_LENGTH
        + currentPattern * PATTERN_TABLE_ENTRY_LENGTH;


  Animation::Encoding encodingType = (Animation::Encoding)pgm_read_byte(patternEntryAddress + ENCODING_TYPE_OFFSET);
  
  PGM_P frameData =  (PGM_P)pgm_read_word(patternEntryAddress + FRAME_DATA_OFFSET);
  uint16_t frameCount = pgm_read_word(patternEntryAddress + FRAME_COUNT_OFFSET);
  frameDelay = pgm_read_word(patternEntryAddress + FRAME_DELAY_OFFSET);

  pattern.init(frameCount, frameData, encodingType, ledCount);
}

// Called when the button is both pressed and released.
ISR(PCINT0_vect){
  buttonState = !(PINB & (1 << PINB6)); // Reading state of the PB6 (remember that HIGH == released)
  
  if(buttonState) {
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
  if(buttonDebounced == false) {
    buttonDebounced = true;
    lastBrightness = brightness;
    brightness = (brightness + 1) % BRIGHT_STEP_COUNT;
    LEDS.setBrightness(brightnesSteps[brightness]);
  }
  
  // If we've waited long enough, switch the pattern
  // TODO: visual indicator
  buttonPressTime = millis() - buttonDownTime;
  if(buttonPressTime > BUTTON_PATTERN_SWITCH_TIME) {
    // first unroll the brightness!
    brightness = lastBrightness;
    LEDS.setBrightness(brightnesSteps[brightness]);
    
    loadPattern((currentPattern+1)%patternCount);
    
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
  patternCount = pgm_read_byte(PATTERN_TABLE_ADDRESS + PATTERN_COUNT_OFFSET);
  ledCount     = pgm_read_word(PATTERN_TABLE_ADDRESS + LED_COUNT_OFFSET);
  
  // Now, read the first pattern from the table
  // TODO: Read a different pattern?
  loadPattern(0);

  LEDS.addLeds<WS2811, LED_OUT, GRB>(leds, LED_COUNT);
  LEDS.setBrightness(brightnesSteps[brightness]);
  LEDS.show();
}



void loop()
{
  // If'n we get some data, switch to passthrough mode
  if(Serial.available() > 0) {
    serialLoop(leds);
  }
  
  pattern.draw(leds);
  // TODO: More sophisticated wait loop to get constant framerate.
  delay(frameDelay);
}

