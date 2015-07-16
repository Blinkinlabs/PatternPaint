#ifndef PATTERNITEM_H
#define PATTERNITEM_H

#include <QFileInfo>
#include <QListWidgetItem>
#include <QUndoStack>

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
    void setImage(const QImage& newImage);

    /// Get a pointer to the undo stack. This is used to wire the undo stack
    /// into the main window gui.
    QUndoStack* getUndoStack() { return &undoStack; }

    /// Get a reference to the current image
    /// @return Pattern image data
    const QImage& getImage() const { return image; }

    /// Get the file information for this pattern, if any
    /// @return File information for the file. If the file has not been saved,
    ///         this returns an empty fileInfo object.
    const QFileInfo& getFileInfo() const { return fileInfo; }

    /// Set the file
    void setFileInfo(const QFileInfo& fileName) { fileInfo = fileName; }

    /// Resize the image
    /// @param patternLength new pattern length
    /// @param ledCount new LED count
    /// @param scale If true, scale the image to fit the new size. Otherwise crop/expand the image.
    void resize(int newPatternLength, int newLedCount, bool scale);

    /// Flip the pattern horizontally
    void flipHorizontal();

    /// Flip the pattern vertically
    void flipVertical();

    /// Clear the image
    void clear();

    /// Apply changes to the pattern
    void applyInstrument(QImage& update);

    /// Test if the pattern has unsaved changes
    /// @return true if the pattern has unsaved changes
    bool getModified() const { return modified; }

    /// Set the unsaved pattern state
    /// TODO: Delete me
    /// @param newModified New modified state.
    void setModified(bool newModified);


private:
    QUndoStack  undoStack;      ///< Undo stack for this pattern
    QImage  image;              ///< Image representation of the pattern
    QFileInfo fileInfo;         ///< Image filename

    bool modified;              ///< True if the image has been modified since last save

    void pushUndoState();
};

#endif // PATTERNITEM_H
