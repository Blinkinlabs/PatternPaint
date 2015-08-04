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

void MatrixDisplay::deleteFrame(int newFrame) {
    // TODO: Design patternItem() around a QData() array instead of an image, drop this junk.

    if(patternItem->getImage().width() < 2) {
        return;
    }

    if(newFrame > patternItem->getImage().width() || newFrame < 0) {
        return;
    }

    QImage originalImage = patternItem->getImage();
    QImage newImage(originalImage.width()-1,
                    originalImage.height(),
                    QImage::Format_ARGB32_Premultiplied);

    // copy data from the original image over
//    // Initialize the pattern to a blank canvass
//    newImage.fill(COLOR_CANVAS_DEFAULT);
    QPainter painter(&newImage);

    if(newFrame == 0) {
        // Crop the front from the original image
        painter.drawImage(0,0,originalImage,1,0,-1,-1);
    }
    else if(newFrame == originalImage.width()-1) {
        // Crop the back from the original image
        painter.drawImage(0,0,originalImage,0,0,-1,-1);
    }
    else {
        // Crop somewhere in the middle
        painter.drawImage(0,0,originalImage,0,0,newFrame,-1);
        painter.drawImage(newFrame,0,originalImage,newFrame+1,0,-1,-1);
    }

    // TODO: This pushes two undo operations...
    patternItem->resize(newImage.width(), newImage.height(), false);
    patternItem->applyInstrument(newImage);

    // and switch to the new frame
    if(newFrame >= newImage.width()) {
        newFrame = newImage.width() -1;
    }
    frame = newFrame;
}

void MatrixDisplay::addFrame(int newFrame) {
    // TODO: Design patternItem() around a QData() array instead of an image, drop this junk.

    if(newFrame > patternItem->getImage().width() || newFrame < 0) {
        return;
    }

    QImage originalImage = patternItem->getImage();
    QImage newImage(originalImage.width()+1,
                    originalImage.height(),
                    QImage::Format_ARGB32_Premultiplied);

    // copy data from the original image over
    // Initialize the pattern to a blank canvass
    newImage.fill(COLOR_CANVAS_DEFAULT);
    QPainter painter(&newImage);

    if(newFrame == originalImage.width()-1) {
        // Crop the back from the original image
        painter.drawImage(0,0,originalImage,0,0,-1,-1);
    }
    else {
        // Crop somewhere in the middle
        painter.drawImage(0,0,originalImage,0,0,newFrame+1,-1);
        painter.drawImage(newFrame+2,0,originalImage,newFrame+1,0,-1,-1);
    }

    // TODO: This pushes two undo operations...
    patternItem->resize(newImage.width(), newImage.height(), false);
    patternItem->applyInstrument(newImage);

    frame = newFrame+1;
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
