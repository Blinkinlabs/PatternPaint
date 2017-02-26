#include "BlinkyTape.h"

#include <Arduino.h>

#define PIXEL_DATA_SIZE 3

extern CLEDController* controller;

void serialLoop(CRGB* leds) {
  uint16_t currentPixel;            // Pixel that should be written to next
  uint8_t buffer[PIXEL_DATA_SIZE];  // Buffer to store incoming pixel data
  uint8_t bufferIndex = 0;          // Write index into the pixel buffer
  uint8_t c;
  
  // Wait for serial data
  while(true) {
    if (Serial.available() > 0) {
      c = Serial.read();
      if (c == 255) {
        
        // Check if we need to enable more output LEDs
        if(currentPixel > controller->size()) {
          controller->setLeds(leds, currentPixel);
        }
        
        LEDS.show();
        currentPixel = 0;
        bufferIndex = 0;

      } else {
        
        buffer[bufferIndex++] = c;
        if (bufferIndex == PIXEL_DATA_SIZE) {
          if(currentPixel < MAX_LEDS) {
            leds[currentPixel] = CRGB(buffer[0], buffer[1], buffer[2]);
            currentPixel++;
          }
          bufferIndex = 0;
        }
      }
    }
  }
}
