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

const QImage& TimelineDisplay::getFrameData() {
    return patternItem->getImage();
}

void TimelineDisplay::applyInstrument(const QImage &instrumentFrameData) {
    patternItem->applyInstrument(instrumentFrameData);
}
