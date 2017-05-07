#ifndef PATTERNSCROLLMODEL_H
#define PATTERNSCROLLMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>
#include <QImage>
#include <QUndoStack>
#include "patternmodel.h"

class PatternScrollUndoCommand;

/// Model for a pattern made from a single QImage that the fixture scrolls across
class PatternScrollModel : public PatternModel
{
    Q_OBJECT
public:

    struct State {
        QImage image;
        QSize frameSize;            ///< Resolution of this pattern, in pixels
        float frameSpeed;           ///< Speed to play back the pattern
        QString fileName;           ///< Filename for the pattern
        bool modified;              ///< True if the pattern has been changed since last save
    };

    PatternScrollModel(QSize frameSize, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    Qt::DropActions supportedDropActions() const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

    QUndoStack *getUndoStack()
    {
        return &undoStack;
    }

    friend class PatternScrollUndoCommand;

    bool writeDataToStream(QDataStream& stream);
    bool readDataFromStream(QDataStream& stream);

private:
    QUndoStack undoStack;       ///< Undo stack for this pattern

    State state;

    void pushUndoState();

    void applyUndoState(State newState);
};

#endif // PATTERNSCROLLMODEL_H
