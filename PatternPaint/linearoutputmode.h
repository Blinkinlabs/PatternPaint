#ifndef TIMELINEDISPLAY_H
#define TIMELINEDISPLAY_H

#include "outputmode.h"


class LinearOutputMode : public OutputMode {
public:

    bool hasFixedLedCount() const { return false; }
    int getFixedLedCount() const { return 0; }

    void setSource(PatternItem* newPatternItem);
    bool hasPatternItem() const;

    bool showPlaybackIndicator() const { return true;}

    int getFrameCount() const;

    void setFrameIndex(int newFrame);
    int getFrameIndex() const;

    void deleteFrame(int newFrame);
    void addFrame(int newFrame);

    const QImage& getFrame();
    void applyInstrument(const QImage& instrumentFrameData) ;

    const QImage& getStreamImage();

private:
    PatternItem* patternItem;
    int frame;
};

#endif // TIMELINEDISPLAY_H
