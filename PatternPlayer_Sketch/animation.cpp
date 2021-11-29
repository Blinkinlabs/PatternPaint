#include "Animation.h"

Animation::Animation()
{
  init(0, NULL, RGB24, 0, 0, 0);
}

Animation::Animation(uint16_t frameCount_,
                     PGM_P frameData_,
                     Encoding encoding_,
                     uint16_t ledCount_,
                     uint16_t frameDelay_,
                     uint16_t repeatCount_
                     )
{
  init(frameCount_, frameData_, encoding_, ledCount_, frameDelay_, repeatCount_);
  reset();
}

void Animation::init(uint16_t frameCount_,
                     PGM_P frameData_,
                     Encoding encoding_,
                     uint16_t ledCount_,
                     uint16_t frameDelay_,
                     uint16_t repeatCount_)
{
  frameCount = frameCount_;
  frameData = frameData_;
  encoding = encoding_;
  ledCount = ledCount_;
  frameDelay = frameDelay_;
  repeatCount = repeatCount_;

  switch(encoding) {
    case RGB24:
      drawFunction = &Animation::drawRgb24;
      break;

    case RGB565_RLE:
      drawFunction = &Animation::drawRgb565_RLE;
      break;

#ifdef SUPPORTS_PALLETE_ENCODING
    case INDEXED:
      drawFunction = &Animation::drawIndexed;
      loadColorTable();
      break;

    case INDEXED_RLE:
      drawFunction = &Animation::drawIndexed_RLE;
      loadColorTable();
      break;
#endif
  }

  reset();
}
 
#ifdef SUPPORTS_PALLETE_ENCODING
void Animation::loadColorTable() {
  colorTableEntries = pgm_read_byte(frameData) + 1;

  for(int i = 0; i < colorTableEntries; i++) {
    colorTable[i] = CRGB(pgm_read_byte(frameData + 1 + i*3    ),
                         pgm_read_byte(frameData + 1 + i*3 + 1),
                         pgm_read_byte(frameData + 1 + i*3 + 2));
  }
}
#endif

void Animation::reset() {
  frameIndex = 0;
}

void Animation::draw(struct CRGB strip[]) {
  (this->*drawFunction)(strip);

  LEDS.show();
  
  frameIndex = (frameIndex + 1)%frameCount;
  if(frameIndex == 0) {
    if(repeatCount > 0) {
      repeatCount--;
    }
  }
};

uint16_t Animation::getLedCount() const {
  return ledCount;
}

uint16_t Animation::getFrameCount() const {
  return frameCount;
}

uint16_t Animation::getFrameDelay() const {
  return frameDelay;
}

uint16_t Animation::getRepeatCount() const {
  return repeatCount;
}

void Animation::drawRgb24(struct CRGB strip[]) {
  currentFrameData = frameData
    + frameIndex*ledCount*3;  // Offset for current frame
  
  for(uint16_t i = 0; i < ledCount; i++) {
    strip[i] = CRGB(pgm_read_byte(currentFrameData + i*3    ),
                    pgm_read_byte(currentFrameData + i*3 + 1),
                    pgm_read_byte(currentFrameData + i*3 + 2));
  }
}

void Animation::drawRgb565_RLE(struct CRGB strip[]) {
  if(frameIndex == 0) {
    currentFrameData = frameData;
  }

  // Read runs of RLE data until we get enough data.
  uint16_t count = 0;
  while(count < ledCount) {
    uint8_t run_length = pgm_read_byte(currentFrameData);
    uint8_t upperByte = pgm_read_byte(currentFrameData + 1);
    uint8_t lowerByte = pgm_read_byte(currentFrameData + 2);
    
    uint8_t r = ((upperByte & 0xF8)     );
    uint8_t g = ((upperByte & 0x07) << 5)
              | ((lowerByte & 0xE0) >> 3);
    uint8_t b = ((lowerByte & 0x1F) << 3);
    
    for(uint8_t i = 0; i < run_length; i++) {
      strip[count + i] = CRGB(r,g,b);
    }
    
    count += run_length;
    currentFrameData += 3;
  }
};

#ifdef SUPPORTS_PALLETE_ENCODING
void Animation::drawIndexed(struct CRGB strip[]) {
  currentFrameData = frameData
    + 1 + 3*colorTableEntries   // Offset for color table
    + frameIndex*ledCount;      // Offset for current frame
  
  for(uint16_t i = 0; i < ledCount; i++) {
    strip[i] = colorTable[pgm_read_byte(currentFrameData + i)];
  }
}

void Animation::drawIndexed_RLE(struct CRGB strip[]) {
  if(frameIndex == 0) {
    currentFrameData = frameData
      + 1 + 3*colorTableEntries;   // Offset for color table
  }

  // Read runs of RLE data until we get enough data.
  uint16_t count = 0;
  while(count < ledCount) {
    uint8_t run_length = pgm_read_byte(currentFrameData++);
    uint8_t colorIndex = pgm_read_byte(currentFrameData++);
    
    for(uint8_t i = 0; i < run_length; i++) {
      strip[count++] = colorTable[colorIndex];
    }
  }
};
#endif
