#ifndef MATRIXDISPLAY_H
#define MATRIXDISPLAY_H

#include "outputmode.h"

class MatrixOutputMode : public OutputMode {
public:
    MatrixOutputMode(int height, int width);

    bool hasFixedLedCount() const { return true; }
    int getFixedLedCount() const { return height*width; }

    void setSource(PatternItem* newPatternItem);
    bool hasPatternItem() const;

    bool showPlaybackIndicator() const {return false;}

    int getFrameCount() const;

    void setFrameIndex(int newFrame);
    int getFrameIndex() const;

    void deleteFrame(int newFrame);
    void addFrame(int newFrame);

    const QImage& getFrame();
    void applyInstrument(const QImage& instrumentFrameData) ;

    const QImage& getStreamImage();

private:
    const int height;
    const int width;
    PatternItem* patternItem;
    QImage frameData;
    int frame;
    QImage streamImage;
};

#endif // MATRIXDISPLAY_H
