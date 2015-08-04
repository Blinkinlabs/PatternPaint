#include "timelinedisplay.h"

#include <QPainter>

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,255)

void TimelineDisplay::setSource(PatternItem *newPatternItem) {
    patternItem = newPatternItem;
}

bool TimelineDisplay::hasPatternItem() const {
    return patternItem != NULL;
}

void TimelineDisplay::setFrameIndex(int newFrame) {
    frame = newFrame;
}

int TimelineDisplay::getFrameIndex() const {
    return frame;
}

void TimelineDisplay::deleteFrame(int newFrame) {
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

void TimelineDisplay::addFrame(int newFrame) {
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

const QImage& TimelineDisplay::getFrameData() {
    return patternItem->getImage();
}

void TimelineDisplay::applyInstrument(const QImage &instrumentFrameData) {
    patternItem->applyInstrument(instrumentFrameData);
}
