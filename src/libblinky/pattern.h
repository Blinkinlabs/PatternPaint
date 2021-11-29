#ifndef PATTERNITEM_H
#define PATTERNITEM_H

#include "libblinkyglobal.h"

#include <QFileInfo>
#include <QListWidgetItem>
#include <QUndoStack>
#include <QPointer>
#include <QUuid>
#include "patternmodel.h"

/// Representation of a pattern based on a frame model.
class LIBBLINKY_EXPORT Pattern : public QObject
{
    Q_OBJECT

public:
    enum PatternType {
        Scrolling,
        FrameBased
    };

    /// Constructor for an empty pattern item
    /// @param size Size of the display, in pixels
    /// @param frameCount Length of the pattern, in frames
    Pattern(PatternType type, QSize size, int frameCount, QListWidget *parent = 0);

    /// Set the pattern image directly without resizing or setting an undo state. This
    /// is used by the undocommand and should probably be refactored.
    /// @param newImage Set the pattern to this image
    void applyUndoState(const QImage &newImage);

    /// Get a pointer to the undo stack. This is used to wire the undo stack
    /// into the main window gui.
    QUndoStack *getUndoStack() const;

    /// Check if the animation has a valid filename
    /// @return true if the animation filename has been set
    bool hasFilename() const;

    /// Get the pattern filename, or a default if one is not set
    /// @return Pattern name
    QString getName() const;

    /// Initialize the pattern from an image file
    /// @param newFileInfo URL of file to load
    bool load(const QString &newFileName);

    /// Save the pattern
    /// Note: The file must already have a filename, otherwise use saveAs
    bool save();

    /// Save the pattern to a new file
    /// newFileInfo URL of the new file to save to.
    bool saveAs(const QString newFileName);

    /// Resize the image
    /// @param newSize New size of the pattern, in pixels
    /// @param scale If true, scale the image to fit the new size. Otherwise crop/expand the image.
    void resize(QSize newSize, bool scale);

    /// Get the size of an individual frame
    /// @return Frame size, in pixels
    QSize getFrameSize() const;

    /// Get the number of frames contained in the animation
    /// @return Frame count
    int getFrameCount() const;

    /// Get the speed of the
    float getFrameSpeed() const;

    void setFrameSpeed(float speed);

    int getPatternRepeatCount() const;

    void setPatternRepeatCount(int count);

    /// Get an image representing the current frame
    /// @return an NxN QImage reperesenting the current frame data
    const QImage getFrameImage(int index) const;

    /// Apply changes to the pattern
    void setFrameImage(int index, const QImage &update);

    /// Get an image representing the current image
    /// @return an NxN QImage reperesenting the current frame data
    const QImage getEditImage(int index) const;

    /// Apply changes to the pattern
    void setEditImage(int index, const QImage &update);

    /// Delete the frame at the given index
    /// @param frame Index of the frame to delete
    void deleteFrame(int index);

    /// Insert a frame at the given index
    /// @param frame Index that the frame should be inserted at
    void addFrame(int newFrame);

    /// Test if the pattern has unsaved changes
    /// @return true if the pattern has unsaved changes
    bool getModified() const;

    /// Get the underlying data model (for connection to a view)
    PatternModel *getModel() const;

    /// Get the UUID for this pattern
    const QUuid getUuid() const
    {
        return uuid;
    }

    /// True if the pattern editor should show a playback indicator for ths
    /// pattern type
    /// TODO: Delete this
    bool hasPlaybackIndicator() const
    {
        return playbackIndicator;
    }

    bool hasTimeline() const
    {
        return timeline;
    }

private:
    QPointer<PatternModel> model;   ///< Storage container for the images

    // TODO: Push these into the model?
    PatternType type;
    bool playbackIndicator;
    bool timeline;

    // TODO: Figure out a better way to store/copy patterns?
    // Difficult to do so now because they have to be tied into
    // the undo and event notification stacks.
    QUuid uuid;
};

#endif // PATTERNITEM_H
