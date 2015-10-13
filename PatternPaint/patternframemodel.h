#ifndef PATTERNFRAMEMODEL_H
#define PATTERNFRAMEMODEL_H

#include <QAbstractListModel>
#include <QImage>

/// Model for a pattern made from a list of QImage frames
class PatternFrameModel : public QAbstractListModel
{
    Q_OBJECT
public:
    PatternFrameModel();

    PatternFrameModel(QSize size, QObject *parent = 0) :
        QAbstractListModel(parent),
        size(size) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

    /// Get the size of the pattern frame
    const QSize getSize() const {return size;}

    /// Change the pattern frame size
    void resize(QSize newSize, bool scale);

//    /// Get the pattern filename, or a default if one is not set
//    /// @return Pattern name
//    QString getPatternName() const;

//    /// Initialize the pattern from an image file
//    /// @param newFileInfo URL of file to load
//    bool load(const QFileInfo& newFileInfo);

//    /// Save the pattern
//    /// Note: The file must already have a filename, otherwise use saveAs
//    bool save();

//    /// Save the pattern to a new file
//    /// newFileInfo URL of the new file to save to.
//    bool saveAs(const QFileInfo& newFileInfo);

private:
    QList<QImage> frames;
    QSize size;
};

#endif // PATTERNFRAMEMODEL_H
