#include "timelinedisplay.h"

void TimelineDisplay::setSource(PatternItem *newPatternItem) {
    patternItem = newPatternItem;
}

bool TimelineDisplay::hasPatternItem() const {
    return patternItem != NULL;
}

void TimelineDisplay::setFrame(int newFrame) {
    frame = newFrame;
}

int TimelineDisplay::getFrame() const {
    return frame;
}

const QImage& TimelineDisplay::getFrameData() {
    return patternItem->getImage();
}

void TimelineDisplay::applyInstrument(const QImage &instrumentFrameData) {
    patternItem->applyInstrument(instrumentFrameData);
}
