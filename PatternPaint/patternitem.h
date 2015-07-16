#ifndef PATTERNITEM_H
#define PATTERNITEM_H

#include <QFileInfo>
#include <QListWidgetItem>
#include <QUndoGroup>
#include <QUndoStack>

class PatternItem : public QListWidgetItem
{
public:
    enum Roles {
        PreviewImage = Qt::UserRole + 1,
        Modified,
        PatternSize
    };

    explicit PatternItem(int patternLength, int ledCount, QListWidget* parent = 0);

    explicit PatternItem(int patternLength, int ledCount, QImage newImage, QListWidget *parent = 0);

    QVariant data(int role) const;
    void setData(int role, const QVariant& value);

    void setImage(const QImage&);
    const QImage& getImage() const { return image; }
    QImage* getImagePointer() { return &image; }

    QUndoStack* getUndoStack() { return &undoStack; }
    void pushUndoState();
    void popUndoState();

    QFileInfo getFileInfo() const { return fileInfo; }
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

    /// Apply changes to the pattern
    void applyInstrument(QImage& update);

    /// Test if the pattern has unsaved changes
    /// @return true if the pattern has unsaved changes
    bool getModified() const { return modified; }

    /// Set the unsaved pattern state
    /// TODO: Delete me
    /// @param newModified New modified state.
    void setModified(bool newModified);

    void clear();

private:
    QUndoStack  undoStack;      ///< Undo stack for this pattern
    QImage  image;              ///< Image representation of the pattern
    QFileInfo fileInfo;         ///< Image filename

    bool modified;              ///< True if the image has been modified since last save

signals:
//    void changed(bool);

public slots:
};

#endif // PATTERNITEM_H
