#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>
#include <FastLED.h>

class Animation {
 public:
  typedef enum {
    RGB24 = 0,
    RGB565_RLE = 1,
#ifdef SUPPORTS_PALLETE_ENCODING
    INDEXED = 2,
    INDEXED_RLE = 3
#endif
  } Encoding;

  typedef enum {
    MANUAL = 0,       // Load next pattern only when user presses button
    LOOP_COUNT = 1,   // 
    TIME = 2,
  } AdvanceMode;

  // Initialize the animation with no data. This is intended for the case
  // where the animation will be re-initialized from a memory structure in ROM
  // after the sketch starts.
  Animation();

  // Initialize the animation
  // @param frameCount Number of frames in this animation
  // @param frameData Pointer to the frame data. Format of this data is encoding-specficic
  // @param encoding Method used to encode the animation data
  // @param ledCount Number of LEDs in the strip
  // @param frameDelay Number of milliseconds to wait between frames
  // @param repeatCount Number of times to repeat pattern before advancing
  Animation(uint16_t frameCount,
            PGM_P frameData,
            Encoding encoding,
            uint16_t ledCount,
            uint16_t frameDelay,
            uint16_t repeatCount);

  // Re-initialize the animation with new information
  // @param frameCount Number of frames in this animation
  // @param frameData Pointer to the frame data. Format of this data is encoding-specficic
  // @param encoding Method used to encode the animation data
  // @param ledCount Number of LEDs in the strip
  // @param frameDelay Number of milliseconds to wait between frames
  // @param repeatCount Number of times to repeat pattern before advancing
  void init(uint16_t frameCount,
            PGM_P frameData,
            Encoding encoding,
            uint16_t ledCount,
            uint16_t frameDelay,
            uint16_t repeatCount);
 
  // Reset the animation, causing it to start over from frame 0.
  void reset();
  
  // Draw the next frame of the animation
  // @param strip[] LED strip to draw to.
  void draw(struct CRGB strip[]);

  uint16_t getLedCount() const;
  uint16_t getFrameCount() const;
  uint16_t getFrameDelay() const;
  uint16_t getRepeatCount() const;

 private:
  uint16_t ledCount;              // Number of LEDs in the strip
  uint16_t frameCount;            // Number of frames in this animation
  uint16_t frameDelay;            // Milliseconds to wait between frames
  uint16_t repeatCount;           // Number of times to repeat pattern before advancing

  Encoding encoding;              // Encoding type
  PGM_P frameData;           // Pointer to the begining of the frame data
  
  uint16_t frameIndex;            // Current animation frame
  PGM_P currentFrameData;    // Pointer to the current position in the frame data

#ifdef SUPPORTS_PALLETE_ENCODING
  uint8_t colorTableEntries;      // Number of entries in the color table, minus 1 (max 255)
  struct CRGB colorTable[256];    // Color table

  void loadColorTable();          // Load the color table from memory
#endif

  typedef void (Animation::*DrawFunction)(struct CRGB strip[]);
  DrawFunction drawFunction;

  void drawRgb24(struct CRGB strip[]);
  void drawRgb565_RLE(struct CRGB strip[]);

#ifdef SUPPORTS_PALLETE_ENCODING
  void drawIndexed(struct CRGB strip[]);
  void drawIndexed_RLE(struct CRGB strip[]);
#endif
};

#endif
