#ifndef PATTERNFRAMEMODEL_H
#define PATTERNFRAMEMODEL_H

#include <QAbstractListModel>
#include <QImage>
#include <QUndoStack>

/// Model for a pattern made from a list of QImage frames
class PatternFrameModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        FrameData = Qt::UserRole,
    };

    PatternFrameModel(QSize size, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    Qt::DropActions supportedDropActions() const;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

    /// TODO: Move the specializations to a delegate?

    QUndoStack* getUndoStack() {return &undoStack;}
    void applyUndoState(QList<QImage> &newFrames, QSize newSize);

    /// Get the size of the pattern frame
    const QSize getSize() const {return size;}

    /// Change the pattern frame size
    void resize(QSize newSize, bool scale);

    QList<QImage> getFrames() const {return frames;}
    int getFrameCount() const {return frames.size();}

private:
    QList<QImage> frames;
    QUndoStack  undoStack;      ///< Undo stack for this pattern
    QSize size;                 ///< Resolution of this pattern, in pixels
    void pushUndoState();
    bool pushUndo;

    void setPushUndo(bool enable) {pushUndo = enable;}
};

#endif // PATTERNFRAMEMODEL_H
