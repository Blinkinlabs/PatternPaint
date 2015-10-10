#include "matrixoutputmode.h"

#include <QPainter>

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,255)

MatrixOutputMode::MatrixOutputMode(QSize size) :
    matrixSize(size),
    patternItem(NULL),
    frame(0)
{
}

void MatrixOutputMode::setSource(PatternItem *newPatternItem) {
    patternItem = newPatternItem;

    if(patternItem != NULL) {
        setFrameIndex(frame);
    }
}

bool MatrixOutputMode::hasPatternItem() const {
    return patternItem != NULL;
}

QSize MatrixOutputMode::getDisplaySize() const {
    return matrixSize;
}

void MatrixOutputMode::setDisplaySize(QSize newSize) {
    if(matrixSize == newSize) {
        return;

    }

    matrixSize = newSize;
    patternItem->resize(matrixSize, true);
}

void MatrixOutputMode::setFrameIndex(int index) {
    if(frame < 0) {
        frame = 0;
    }
    if(frame > getFrameCount()) {
        frame = getFrameCount()-1;
    }

    frame = index;
}

int MatrixOutputMode::getFrameCount() const {
    return patternItem->getFrameCount();
}

const QImage& MatrixOutputMode::getFrame() {
    return patternItem->getFrame(frame);
}

void MatrixOutputMode::deleteFrame(int index) {
    patternItem->deleteFrame(index);
}

void MatrixOutputMode::addFrame(int index) {
    patternItem->addFrame(index);
}

void MatrixOutputMode::applyInstrument(const QImage &instrumentFrameData) {
    QImage outputImage(patternItem->getImage());

    // TODO: This is an inefficient way of achieving this.
    for(int x = 0; x < instrumentFrameData.width(); x++) {
        for(int y = 0; y < instrumentFrameData.height(); y++) {
//            outputImage.setPixel(frame*width + x, y,
//                                 instrumentFrameData.pixel(x, x%2?height-1-y:y));
            outputImage.setPixel(frame*matrixSize.width() + x, y,
                                 instrumentFrameData.pixel(x, y));
        }
    }

    patternItem->applyInstrument(outputImage);
}

const QImage &MatrixOutputMode::getStreamImage() {
    streamImage = QImage(getFrameCount(),matrixSize.width()*matrixSize.height(),QImage::Format_ARGB32_Premultiplied);

    // TODO: This is an inefficient way of achieving this.
    for(int frame = 0; frame < getFrameCount(); frame++) {
        for(int x = 0; x < matrixSize.width(); x++) {
            for(int y = 0; y < matrixSize.height(); y++) {
                streamImage.setPixel(
                            frame,
                            x*matrixSize.height()+(x%2?matrixSize.height()-1-y:y),
                            patternItem->getImage().pixel(frame*matrixSize.width() + x, y)
                            );
            }
        }
    }

    return streamImage;
}
