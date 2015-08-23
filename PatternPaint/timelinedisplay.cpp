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
    if(frame < 0) {
        frame = 0;
    }
    if(frame > getFrameCount()) {
        frame = getFrameCount()-1;
    }

    frame = newFrame;
}

int TimelineDisplay::getFrameCount() const {
    return patternItem->getImage().width();
}

int TimelineDisplay::getFrameIndex() const {
    return frame;
}

const QImage& TimelineDisplay::getFrame() {
    return patternItem->getImage();
}

void TimelineDisplay::deleteFrame(int newFrame) {
    if(getFrameCount() < 2) {
        return;
    }

    if(newFrame > getFrameCount() || newFrame < 0) {
        return;
    }

    QImage newImage(patternItem->getImage().width()-1,
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
        painter.drawImage(0,0,patternItem->getImage(),0,0,newFrame,-1);
        painter.drawImage(newFrame,0,patternItem->getImage(),newFrame+1,0,-1,-1);
    }

    // TODO: This pushes two undo operations...
    patternItem->resize(newImage.width(), newImage.height(), false);
    patternItem->applyInstrument(newImage);
}

void TimelineDisplay::addFrame(int newFrame) {
    // TODO: Design patternItem() around a QData() array instead of an image, drop this junk.

    if(newFrame > getFrameCount() || newFrame < 0) {
        return;
    }

    QImage newImage(patternItem->getImage().width()+1,
                    patternItem->getImage().height(),
                    QImage::Format_ARGB32_Premultiplied);

    // copy data from the original image over
    // Initialize the pattern to a blank canvass
    newImage.fill(COLOR_CANVAS_DEFAULT);
    QPainter painter(&newImage);

    if(newFrame == patternItem->getImage().width()-1) {
        // Crop the back from the original image
        painter.drawImage(0,0,patternItem->getImage(),0,0,-1,-1);
    }
    else {
        // Crop somewhere in the middle
        painter.drawImage(0,0,patternItem->getImage(),0,0,newFrame+1,-1);
        painter.drawImage(newFrame+2,0,patternItem->getImage(),newFrame+1,0,-1,-1);
    }

    // TODO: This pushes two undo operations...
    patternItem->resize(newImage.width(), newImage.height(), false);
    patternItem->applyInstrument(newImage);
}

void TimelineDisplay::applyInstrument(const QImage &instrumentFrameData) {
    patternItem->applyInstrument(instrumentFrameData);
}

const QImage &TimelineDisplay::getStreamImage() {
    return patternItem->getImage();
}
