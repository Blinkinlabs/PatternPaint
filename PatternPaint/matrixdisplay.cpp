#include "matrixdisplay.h"

#include <QPainter>

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,255)

MatrixDisplay::MatrixDisplay(int height, int width) :
    height(height),
    width(width),
    patternItem(NULL)
{}

void MatrixDisplay::setSource(PatternItem *newPatternItem) {
    patternItem = newPatternItem;
}

bool MatrixDisplay::hasPatternItem() const {
    return patternItem != NULL;
}


void MatrixDisplay::setFrameIndex(int newFrame) {
    frame = newFrame;
}

int MatrixDisplay::getFrameIndex() const {
    return frame;
}

const QImage& MatrixDisplay::getFrameData() {
    frameData = QImage(width,height,QImage::Format_ARGB32_Premultiplied);

    // TODO: This is an inefficient way of achieving this.
    for(int x = 0; x < frameData.width(); x++) {
        for(int y = 0; y < frameData.height(); y++) {
            frameData.setPixel(x, x%2?height-1-y:y,
                              patternItem->getImage().pixel(frame, x* frameData.height() + y));
        }
    }

    return frameData;
}

void MatrixDisplay::applyInstrument(const QImage &instrumentFrameData) {
    QImage outputImage(patternItem->getImage());

    // TODO: This is an inefficient way of achieving this.
    for(int x = 0; x < instrumentFrameData.width(); x++) {
        for(int y = 0; y < instrumentFrameData.height(); y++) {
            outputImage.setPixel(frame, x* instrumentFrameData.height() + y,
                                 instrumentFrameData.pixel(x, x%2?height-1-y:y));
        }
    }

    patternItem->applyInstrument(outputImage);
}
