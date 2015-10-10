#include "linearoutputmode.h"

#include <QPainter>

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,255)

LinearOutputMode::LinearOutputMode(QSize size) :
    size(size) {
}

void LinearOutputMode::setSource(PatternItem *newPatternItem) {
    patternItem = newPatternItem;
}

bool LinearOutputMode::hasPatternItem() const {
    return patternItem != NULL;
}

QSize LinearOutputMode::getDisplaySize() const
{
    return size;
}

void LinearOutputMode::setDisplaySize(QSize newSize)
{
    if(size == newSize) {
        return;
    }

    size = newSize;
    patternItem->resize(size, true);
}

void LinearOutputMode::setFrameIndex(int index) {
    if(frame < 0) {
        frame = 0;
    }
    if(frame > getFrameCount()) {
        frame = getFrameCount()-1;
    }

    frame = index;
}

int LinearOutputMode::getFrameCount() const {
    return patternItem->getFrameCount();
}

const QImage& LinearOutputMode::getFrame() {
    return patternItem->getImage();
    // return patternItem->getFrame(frame);
}

void LinearOutputMode::deleteFrame(int index) {
    patternItem->deleteFrame(index);
}

void LinearOutputMode::addFrame(int index) {
    patternItem->addFrame(index);
}

void LinearOutputMode::applyInstrument(const QImage &instrumentFrameData) {
    patternItem->applyInstrument(instrumentFrameData);
}

const QImage &LinearOutputMode::getStreamImage() {
    return patternItem->getImage();
}
