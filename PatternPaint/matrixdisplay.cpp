#include "matrixdisplay.h"

#include <QPainter>

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,255)

MatrixDisplay::MatrixDisplay(int height, int width) :
    height(height),
    width(width),
    patternItem(NULL),
    frame(0)
{}

void MatrixDisplay::setSource(PatternItem *newPatternItem) {
    patternItem = newPatternItem;

    if(patternItem != NULL) {
        setFrameIndex(frame);
    }
}

bool MatrixDisplay::hasPatternItem() const {
    return patternItem != NULL;
}


void MatrixDisplay::setFrameIndex(int newFrame) {
    if(frame < 0) {
        frame = 0;
    }
    if(frame > getFrameCount()) {
        frame = getFrameCount()-1;
    }

    frame = newFrame;
}

int MatrixDisplay::getFrameCount() const {
    return patternItem->getImage().width()/width;
}

int MatrixDisplay::getFrameIndex() const {
    return frame;
}

const QImage& MatrixDisplay::getFrame() {
    frameData = QImage(width,height,QImage::Format_ARGB32_Premultiplied);

    // TODO: This is an inefficient way of achieving this.
    for(int x = 0; x < frameData.width(); x++) {
        for(int y = 0; y < frameData.height(); y++) {
            frameData.setPixel(x, y,
                              patternItem->getImage().pixel(frame*width + x, y));
        }
    }

    return frameData;
}




void MatrixDisplay::deleteFrame(int newFrame) {
    if(getFrameCount() < 2) {
        return;
    }

    if(newFrame > getFrameCount() || newFrame < 0) {
        return;
    }

    QImage newImage(patternItem->getImage().width()-width,
                    patternItem->getImage().height(),
                    QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&newImage);

    if(newFrame == 0) {
        // Crop the front from the original image
        painter.drawImage(0,0,patternItem->getImage(),1,0,-1,-1);
    }
    else if(newFrame == patternItem->getImage().width()-1) {
        // Crop the back from the original image
        painter.drawImage(0,0,patternItem->getImage(),0,0,-1,-1);
    }
    else {
        // Crop somewhere in the middle
        painter.drawImage(0,0,patternItem->getImage(),0,0,newFrame*width,-1);
        painter.drawImage(newFrame*width,0,patternItem->getImage(),(newFrame+1)*width,0,-1,-1);
    }

    // TODO: This pushes two undo operations...
    patternItem->resize(newImage.width(), newImage.height(), false);
    patternItem->applyInstrument(newImage);
}

void MatrixDisplay::addFrame(int newFrame) {
    if(newFrame > getFrameCount() || newFrame < 0) {
        return;
    }

    QImage newImage(patternItem->getImage().width()+width,
                    patternItem->getImage().height(),
                    QImage::Format_ARGB32_Premultiplied);

    // copy data from the original image over
    // Initialize the pattern to a blank canvass
    newImage.fill(COLOR_CANVAS_DEFAULT);
    QPainter painter(&newImage);

    if(newFrame == getFrameCount()-1) {
        // Add to the end of the image
        painter.drawImage(0,0,patternItem->getImage(),0,0,-1,-1);
    }
    else {
        // Crop somewhere in the middle
        painter.drawImage(0,0,patternItem->getImage(),0,0,(newFrame+1)*width,-1);
        painter.drawImage((newFrame+2)*width,0,patternItem->getImage(),(newFrame+1)*width,0,-1,-1);
    }

    // TODO: This pushes two undo operations...
    patternItem->resize(newImage.width(), newImage.height(), false);
    patternItem->applyInstrument(newImage);
}

void MatrixDisplay::applyInstrument(const QImage &instrumentFrameData) {
    QImage outputImage(patternItem->getImage());

    // TODO: This is an inefficient way of achieving this.
    for(int x = 0; x < instrumentFrameData.width(); x++) {
        for(int y = 0; y < instrumentFrameData.height(); y++) {
//            outputImage.setPixel(frame*width + x, y,
//                                 instrumentFrameData.pixel(x, x%2?height-1-y:y));
            outputImage.setPixel(frame*width + x, y,
                                 instrumentFrameData.pixel(x, y));
        }
    }

    patternItem->applyInstrument(outputImage);
}

const QImage &MatrixDisplay::getStreamImage() {
    streamImage = QImage(getFrameCount(),width*height,QImage::Format_ARGB32_Premultiplied);

    // TODO: This is an inefficient way of achieving this.
    for(int frame = 0; frame < getFrameCount(); frame++) {
        for(int x = 0; x < width; x++) {
            for(int y = 0; y < height; y++) {
                streamImage.setPixel(
                            frame,
                            x*height+(x%2?height-1-y:y),
                            patternItem->getImage().pixel(frame*width + x, y)
                            );
            }
        }
    }

    return streamImage;
}
