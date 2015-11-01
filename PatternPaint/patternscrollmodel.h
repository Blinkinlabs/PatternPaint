#ifndef PATTERNSCROLLMODEL_H
#define PATTERNSCROLLMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>
#include <QImage>
#include <QUndoStack>
#include "patternmodel.h"

/// Model for a pattern made from a single QImage that the fixture scrolls across
class PatternScrollModel : public PatternModel
{
    Q_OBJECT
public:

    PatternScrollModel(QSize frameSize, QObject *parent = 0);

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

private:
    QImage image;

    QUndoStack  undoStack;      ///< Undo stack for this pattern
    QSize frameSize;            ///< Resolution of this pattern, in pixels
    QString fileInfo;           ///< Filename for the pattern
    bool modified;              ///< True if the pattern has been changed since last save

    void pushUndoState();
};

#endif // PATTERNSCROLLMODEL_H
