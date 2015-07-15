#ifndef PATTERNITEM_H
#define PATTERNITEM_H

#include <QListWidgetItem>
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

    QVariant data(int role) const;
    void setData(int role, const QVariant& value);

    void setImage(const QImage&);
    const QImage& getImage() const { return img; }

    QImage* getImagePointer() { return &img; }

//    QUndoStack* stack() { return &ustack; }
//    bool isModified() const { return modified; }

    QString fileName() const { return filename; }
    void setFileName(const QString& fileName) { filename = fileName; }

    /// Resize the image
    /// @param patternLength new pattern length
    /// @param ledCount new LED count
    /// @param scale If true, scale the image to fit the new size. Otherwise crop/expand the image.
    void resizeImage(int newPatternLength, int newLedCount, bool scale);

private:
    QUndoStack  ustack;
    QImage  img;
    QSize   psize;
    bool    modified;
    QString filename;
signals:

public slots:
};

#endif // PATTERNITEM_H
