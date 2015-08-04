#ifndef TIMELINEDISPLAY_H
#define TIMELINEDISPLAY_H

#include "displaymodel.h"


class TimelineDisplay : public DisplayModel {
public:
    void setSource(PatternItem* newPatternItem);
    bool hasPatternItem() const;

    bool showPlaybackIndicator() const { return true;}

    void setFrameIndex(int newFrame);
    int getFrameIndex() const;

    const QImage& getFrameData();
    void applyInstrument(const QImage& instrumentFrameData) ;

private:
    PatternItem* patternItem;
    int frame;
};

#endif // TIMELINEDISPLAY_H
