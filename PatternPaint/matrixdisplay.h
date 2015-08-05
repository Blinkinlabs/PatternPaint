#ifndef MATRIXDISPLAY_H
#define MATRIXDISPLAY_H

#include "displaymodel.h"

class MatrixDisplay : public DisplayModel {
public:
    MatrixDisplay(int height, int width);

    bool hasFixedLedCount() const { return true; }
    int getFixedLedCount() const { return height*width; }

    void setSource(PatternItem* newPatternItem);
    bool hasPatternItem() const;

    bool showPlaybackIndicator() const {return false;}

    void setFrameIndex(int newFrame);
    int getFrameIndex() const;

    const QImage& getFrameData();
    void applyInstrument(const QImage& instrumentFrameData) ;

private:
    const int height;
    const int width;
    PatternItem* patternItem;
    QImage frameData;
    int frame;
};

#endif // MATRIXDISPLAY_H
