#ifndef LEDDISPLAY
#define LEDDISPLAY

#include <qimage.h>
#include "patternitem.h"

class OutputMode
{
public:
    enum Mode {
        TIMELINE    = 0,     /// Timeline (classic) mode
        MATRIX   = 1,     /// 8x8 Matrix mode
    };

    virtual ~OutputMode() {}

    /// Set the source pattern that we are interacting with
    virtual void setSource(PatternItem* newPatternItem) = 0;

    /// True if this model is attached to a pattern item
    virtual bool hasPatternItem() const = 0;

    /// Get the display geometry
    /// @return size of the display, in LEDs
    virtual QSize getDisplaySize() const = 0;

    /// Change the display geometry
    /// @param size New display size, in LEDs
    virtual void setDisplaySize(QSize size) = 0;

    /// True if the patternEditor should show a playback indication bar
    /// @return True if the pattern editor should show a playback bar
    virtual bool showPlaybackIndicator() const = 0;

    /// Get the number of frames contained in the animation
    /// @return Frame count
    virtual int getFrameCount() const = 0;

    /// Set the index of the active frame
    /// @param newFrame Frame index to interact with
    virtual void setFrameIndex(int newFrame) = 0;

    /// Get the index of the active frame
    /// @return Index of the active fraome
    virtual int getFrameIndex() const = 0;

    /// Get an image representing the current frame
    /// @return an NxN QImage reperesenting the current frame data
    virtual const QImage& getFrame() = 0;

    /// Delete the frame at the given index
    /// @param frame Index of the frame to delete
    virtual void deleteFrame(int index) = 0;

    /// Insert a frame at the given index
    /// @param frame Index that the frame should be inserted at
    virtual void addFrame(int newFrame) = 0;

    /// Apply instrument data to the current frame
    /// @instrumentFrameData NxN QImage to be painted on top of the current frame data
    virtual void applyInstrument(const QImage& instrumentFrameData) = 0;

    /// Get a 2d image that represents the byte stream to send to the LEDs,
    /// where each column represents a frame, and each row an LED output.
    /// @return stream image
    virtual const QImage& getStreamImage() = 0;
};

#endif // LEDDISPLAY

