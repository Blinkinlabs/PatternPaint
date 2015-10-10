#ifndef MATRIXDISPLAY_H
#define MATRIXDISPLAY_H

#include "outputmode.h"

class MatrixOutputMode : public OutputMode {
public:
    MatrixOutputMode(QSize size);

    void setSource(PatternItem* newPatternItem);
    bool hasPatternItem() const;

    QSize getDisplaySize() const;
    void setDisplaySize(QSize size);

    bool showPlaybackIndicator() const {return false;}

    int getFrameCount() const;

    void setFrameIndex(int index);
    int getFrameIndex() const {return frame;}

    void deleteFrame(int index);
    void addFrame(int index);

    const QImage& getFrame();
    void applyInstrument(const QImage& instrumentFrameData) ;

    const QImage& getStreamImage();

private:
    QSize matrixSize;
    PatternItem* patternItem;
    int frame;
    QImage streamImage;
};

#endif // MATRIXDISPLAY_H
