#ifndef PATTERNITEM_H
#define PATTERNITEM_H

#include <QFileInfo>
#include <QListWidgetItem>
#include <QUndoStack>
#include <QPointer>
#include "patternupdatenotifier.h"

/// patternItem is a combination of data storage and some simple operators for a
/// pattern. There is one patternItem per open pattern.
class PatternItem : public QListWidgetItem
{
public:
    enum Roles {
        PreviewImage = Qt::UserRole + 1,
        Modified,
        PatternSize
    };

    /// Constructor for an empty pattern item
    /// @param patternLength Length of the pattern, in frames
    /// @param ledCount Number of LEDs to play this pattern onto
    explicit PatternItem(int patternLength, int ledCount, QListWidget* parent = 0);

    /// Constructor for a pattern based on an image
    /// @param ledCount Number of LEDs to play this pattern onto
    /// @param newImage Image to load into this pattern. Note that the image
    ///         will be resized to fit patternLength and ledCount
    explicit PatternItem(int ledCount, QImage newImage, QListWidget *parent = 0);

    QVariant data(int role) const;
    void setData(int role, const QVariant& value);

    /// Set the pattern image directly without resizing or setting an undo state. This
    /// is used by the undocommand and should probably be refactored.
    /// @param newImage Set the pattern to this image
    void applyUndoState(const QImage& newImage);

    /// Get a pointer to the undo stack. This is used to wire the undo stack
    /// into the main window gui.
    QUndoStack* getUndoStack() { return &undoStack; }

    /// Get a reference to the current image
    /// @return Pattern image data
    const QImage& getImage() const { return image; }

    /// Check if the animation has a valid filename
    /// @return true if the animation filename has been set
    bool hasValidFilename() const { return fileInfo.baseName() != "";}

    /// Get the pattern filename, or a default if one is not set
    /// @return Pattern name
    QString getPatternName() const;

    /// Initialize the pattern from an image file
    /// @param newFileInfo URL of file to load
    bool load(const QFileInfo& newFileInfo);

    /// Save the pattern
    /// Note: The file must already have a filename, otherwise use saveAs
    bool save();

    /// Save the pattern to a new file
    /// newFileInfo URL of the new file to save to.
    bool saveAs(const QFileInfo& newFileInfo);

    /// Replace the current image with one from a new file, but don't
    /// change the filename (for drag&drop operations)
    /// @param newFileInfo URL of file to load
    bool replace(const QFileInfo& newFileInfo);

    /// Resize the image
    /// @param patternLength new pattern length
    /// @param ledCount new LED count
    /// @param scale If true, scale the image to fit the new size. Otherwise crop/expand the image.
    void resize(int newPatternLength, int newLedCount, bool scale);

    /// Apply changes to the pattern
    void applyInstrument(const QImage& update);

    /// Delete the frame at the given index
    /// @param frame Index of the frame to delete
    void deleteFrame(int newFrame);

    /// Insert a frame at the given index
    /// @param frame Index that the frame should be inserted at
    void addFrame(int newFrame);

//    /// Get the number of frames in this pattern
//    /// @return Frame count
//    int getFrameCount() const;

    /// Test if the pattern has unsaved changes
    /// @return true if the pattern has unsaved changes
    bool getModified() const { return modified; }

    /// Set the unsaved pattern state
    /// TODO: Delete me
    /// @param newModified New modified state.
    void setModified(bool newModified);

    /// Register a callback function to be notified when
    /// this data has changed
    void setNotifier(QPointer<PatternUpdateNotifier> newNotifier);

private:
    QUndoStack  undoStack;      ///< Undo stack for this pattern
    QImage  image;              ///< Image representation of the pattern
    QFileInfo fileInfo;         ///< Image filename

    QPointer<PatternUpdateNotifier> notifier; ///< Callback to notify that the data has been updated.

    bool modified;              ///< True if the image has been modified since last save

    void pushUndoState();
};

#endif // PATTERNITEM_H
