#include <FastSPI_LED2.h>
#include <avr/pgmspace.h>
#include <Animation.h>

#define LED_COUNT 60
struct CRGB leds[LED_COUNT];

#ifdef REVB // RevB boards have a slightly different pinout.

#define LED_OUT      5
#define BUTTON_IN    13
#define ANALOG_INPUT A11
#define IO_A         15

#else

#define LED_OUT      13 
#define BUTTON_IN    10
#define ANALOG_INPUT A9
#define IO_A         7
#define IO_B         11

#endif

#define BRIGHT_STEP_COUNT 5
uint8_t brightnesSteps[BRIGHT_STEP_COUNT] = {5,15,40,70,93};
uint8_t brightness = 4;
uint8_t lastButtonState = 1;

Animation pov;

int frameDelay = 30; // Number of ms each frame should be displayed.

void setup()
{  
  Serial.begin(57600);

  LEDS.addLeds<WS2811, LED_OUT, GRB>(leds, LED_COUNT);
  LEDS.showColor(CRGB(0, 0, 0));
  LEDS.setBrightness(brightnesSteps[brightness]);
  LEDS.show();
  
  pinMode(BUTTON_IN, INPUT_PULLUP);
  
  // Read the animation data from the end of the program memory, and construct a new Animation from it.
  int frameCount;
  prog_uint8_t* frameData;

  // These could be whereever, but need to agree with Processing.
  #define CONTROL_DATA_ADDRESS (0x7000 - 6)
  #define FRAME_DATA_ADDRESS   (CONTROL_DATA_ADDRESS)
  #define FRAME_COUNT_ADDRESS  (CONTROL_DATA_ADDRESS + 2)
  #define FRAME_DELAY_ADDRESS  (CONTROL_DATA_ADDRESS + 4)

  frameData  =
  (prog_uint8_t*)((pgm_read_byte(FRAME_DATA_ADDRESS    ) << 8)
                  + (pgm_read_byte(FRAME_DATA_ADDRESS + 1)));
               
  frameCount = (pgm_read_byte(FRAME_COUNT_ADDRESS    ) << 8)
             + (pgm_read_byte(FRAME_COUNT_ADDRESS + 1));
             
  frameDelay = (pgm_read_byte(FRAME_DELAY_ADDRESS    ) << 8)
             + (pgm_read_byte(FRAME_DELAY_ADDRESS + 1));
             
  pov.init(frameCount, frameData, ENCODING_NONE, LED_COUNT);
}


void serialLoop() {
  static int pixelIndex;
  
  unsigned long lastReceiveTime = millis();

  while(true) {

    if(Serial.available() > 2) {
      lastReceiveTime = millis();

      uint8_t buffer[3]; // Buffer to store three incoming bytes used to compile a single LED color

      for (uint8_t x=0; x<3; x++) { // Read three incoming bytes
        uint8_t c = Serial.read();
        
        if (c < 255) {
          buffer[x] = c; // Using 255 as a latch semaphore
        }
        else {
          LEDS.show();
          pixelIndex = 0;
          break;
        }

        if (x == 2) {   // If we received three serial bytes
          leds[pixelIndex] = CRGB(buffer[0], buffer[1], buffer[2]);
          pixelIndex++;
        }
      }
    }
    
    // If we haven't received data in 4 seconds, return to playing back our animation
    if(millis() > lastReceiveTime + 4000) {
      while(Serial.available() > 0) {
        Serial.read();
      }
      return;
    }
  }
}

int led = 13;


void loop()
{
  // If'n we get some data, switch to passthrough mode
  if(Serial.available() > 0) {
    serialLoop();
  }
  
  // Check if the brightness should be adjusted
  uint8_t buttonState = digitalRead(BUTTON_IN);
  if((buttonState != lastButtonState) && (buttonState == 0)) {
    brightness = (brightness + 1) % BRIGHT_STEP_COUNT;
    LEDS.setBrightness(brightnesSteps[brightness]);
  }
  lastButtonState = buttonState;
  
  pov.draw(leds);
  // TODO: More sophisticated wait loop to get constant framerate.
  delay(frameDelay);
}

