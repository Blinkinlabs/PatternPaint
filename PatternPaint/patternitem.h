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

    explicit PatternItem(QListWidget* parent = 0);
    explicit PatternItem(const QString& text);
    QUndoStack* stack() { return &ustack; }
    QVariant data(int role) const;
    void setData(int role, const QVariant& value);
    void setImage(const QImage&);
    const QImage& getImage() const { return img; }
    bool isModified() const { return modified; }
    bool saveToFile(const QString& fileName);
    QString fileName() const { return filename; }
    void setFileName(const QString& fileName) { filename = fileName; }
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
