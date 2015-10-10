#ifndef TIMELINEDISPLAY_H
#define TIMELINEDISPLAY_H

#include "outputmode.h"


class LinearOutputMode : public OutputMode {
public:
    LinearOutputMode(QSize size);

    void setSource(PatternItem* newPatternItem);
    bool hasPatternItem() const;

    QSize getDisplaySize() const;
    void setDisplaySize(QSize newSize);

    bool showPlaybackIndicator() const {return true;}

    int getFrameCount() const;

    void setFrameIndex(int index);
    int getFrameIndex() const {return frame;}

    void deleteFrame(int index);
    void addFrame(int index);

    const QImage& getFrame();
    void applyInstrument(const QImage& instrumentFrameData) ;

    const QImage& getStreamImage();

private:
    PatternItem* patternItem;
    int frame;
    QSize size;
};

#endif // TIMELINEDISPLAY_H
