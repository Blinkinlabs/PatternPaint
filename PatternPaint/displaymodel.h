#ifndef DISPLAYMODEL
#define DISPLAYMODEL

#include <qimage.h>
#include "patternitem.h"

class DisplayModel
{
public:
    /// Set the source pattern that we are interacting with
    virtual void setSource(PatternItem* newPatternItem) = 0;

    /// True if this model is attached to a pattern item
    virtual bool hasPatternItem() const = 0;

    /// Set the index of the active frame
    /// @param newFrame Frame index to interact with
    virtual void setFrame(int newFrame) = 0;

    /// Get the index of the active frame
    /// @return Index of the active fraome
    virtual int getFrame() const = 0;

    /// Get an image representing the current frame
    /// @return an NxN QImage reperesenting the current frame data
    virtual const QImage& getFrameData() = 0;

    /// Apply instrument data to the current frame
    /// @instrumentFrameData NxN QImage to be painted on top of the current frame data
    virtual void applyInstrument(const QImage& instrumentFrameData) = 0;
};

#endif // DISPLAYMODEL

