#ifndef MATRIXDISPLAY_H
#define MATRIXDISPLAY_H

#include "displaymodel.h"

class MatrixDisplay : public DisplayModel {
public:
    MatrixDisplay(int height, int width);

    void setSource(PatternItem* newPatternItem);
    bool hasPatternItem() const;

    void setFrame(int newFrame);
    int getFrame() const;
    int frameCount() const;

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
