#include <Animation.h>

#include <FastLED.h>
#include <avr/pgmspace.h>

// Output pins
#define LED_OUT      13      // LED output signal
#define BUTTON_IN    10      // Pin that the button is connected to
#define ANALOG_INPUT A9      // Analog input on the bottom of the board
#define IO_A         7       // Extra digital input on the bottom of the board
#define IO_B         11      // Extra digital input on the bottom of the board


// Pattern table definitions
#define PATTERN_TABLE_ADDRESS  (0x7000 - 0x80)   // Location of the pattern table in the flash memory
#define PATTERN_TABLE_HEADER_LENGTH     2        // Length of the header
#define PATTERN_TABLE_ENTRY_LENGTH      7        // Length of each entry
  
#define PATTERN_COUNT_OFFSET    0    // Number of patterns in the pattern table
#define LED_COUNT_OFFSET        1    // Number of LEDs in the pattern
 
#define ENCODING_TYPE_OFFSET    0    // Encoding (1 byte)
#define FRAME_DATA_OFFSET       1    // Memory location (2 bytes)
#define FRAME_COUNT_OFFSET      3    // Frame count (2 bytes)
#define FRAME_DELAY_OFFSET      5    // Frame delay (2 bytes)


// LED data array
#define MAX_LEDS 255          // Maximum number of LEDs supported
struct CRGB leds[MAX_LEDS];   // Space to hold the pattern
uint8_t ledCount;             // Actual number of LEDs present

// Pattern information
uint8_t patternCount;         // Number of available patterns
uint8_t patternIndex;         // Index of the current patter
Animation pattern;            // Current pattern
int frameDelay = 30;          // Number of ms each frame should be displayed.

// Brightness selection
#define BRIGHT_STEP_COUNT 5
uint8_t brightnesSteps[BRIGHT_STEP_COUNT] = {5,15,40,70,93};
uint8_t brightness = 4;
uint8_t lastButtonState = 1;


// Read the pattern data from the end of the program memory, and construct a new Pattern from it.
void loadPattern(uint8_t patternIndex) {
  uint16_t patternEntryAddress =
        PATTERN_TABLE_ADDRESS
        + PATTERN_TABLE_HEADER_LENGTH
        + patternIndex * PATTERN_TABLE_ENTRY_LENGTH;


  uint8_t encodingType = pgm_read_byte(patternEntryAddress + ENCODING_TYPE_OFFSET);
  
  uint8_t PROGMEM *frameData  =
  (uint8_t PROGMEM *)((pgm_read_byte(patternEntryAddress + FRAME_DATA_OFFSET    ) << 8)
                + (pgm_read_byte(patternEntryAddress + FRAME_DATA_OFFSET + 1)));

  uint16_t frameCount = (pgm_read_byte(patternEntryAddress + FRAME_COUNT_OFFSET    ) << 8)
                      + (pgm_read_byte(patternEntryAddress + FRAME_COUNT_OFFSET + 1));
             
  frameDelay  = (pgm_read_byte(patternEntryAddress + FRAME_DELAY_OFFSET    ) << 8)
              + (pgm_read_byte(patternEntryAddress + FRAME_DELAY_OFFSET + 1));

  pattern.init(frameCount, frameData, encodingType, ledCount);
}


void setup()
{  
  Serial.begin(57600);
  
  pinMode(BUTTON_IN, INPUT_PULLUP);
  

  // First, load the pattern count and LED geometry from the pattern table
  patternCount = pgm_read_byte(PATTERN_TABLE_ADDRESS + PATTERN_COUNT_OFFSET);
  ledCount     = pgm_read_byte(PATTERN_TABLE_ADDRESS + LED_COUNT_OFFSET);
  
  // Now, read the first pattern from the table
  // TODO: Read a different pattern?
  patternIndex = 0;
  loadPattern(patternIndex);

  LEDS.addLeds<WS2811, LED_OUT, GRB>(leds, ledCount);
  LEDS.showColor(CRGB(0, 0, 0));
  LEDS.setBrightness(brightnesSteps[brightness]);
  LEDS.show();
}


void serialLoop() {
  
  static int pixelIndex;
  uint8_t idx = 0;
  uint8_t buffer[3];
  uint8_t c;
  
  while(true) {
    if (Serial.available() > 0) {
      c = Serial.read();
      if (c == 255) {
	LEDS.show();
	pixelIndex = 0;
	idx = 0;   
	// BUTTON_IN (D10):   07 - 0111
	// EXTRA_PIN_A(D7):          11 - 1011
	// EXTRA_PIN_B (D11):        13 - 1101
	// ANALOG_INPUT (A9): 14 - 1110
      } else {        
        buffer[idx++] = c;
        if (idx == 3) {
          if(pixelIndex == MAX_LEDS) break; // Prevent overflow by ignoring the pixel data beyond LED_COUNT
          leds[pixelIndex] = CRGB(buffer[0], buffer[1], buffer[2]);
          pixelIndex++;
          idx = 0;
        }
      }
    }
  }
}

void loop()
{
  // If'n we get some data, switch to passthrough mode
  if(Serial.available() > 2) {
    serialLoop();
  }
  
  // Check if the brightness should be adjusted
  uint8_t buttonState = digitalRead(BUTTON_IN);
  if((buttonState != lastButtonState) && (buttonState == 0)) {
    brightness = (brightness + 1) % BRIGHT_STEP_COUNT;
    LEDS.setBrightness(brightnesSteps[brightness]);
  }
  lastButtonState = buttonState;
  
  pattern.draw(leds);
  // TODO: More sophisticated wait loop to get constant framerate.
  delay(frameDelay);
}

