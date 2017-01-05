#ifndef PATTERNMODEL
#define PATTERNMODEL

#include <QAbstractListModel>
#include <QObject>
#include <QUndoStack>

/// Interface for accessing a pattern model.
class PatternModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        FrameImage = Qt::UserRole,      // Image data for the given frame
        FrameSize,                      // Size of a frame image
        FrameSpeed,                     // Speed to play back the frame
        FileName,                       // File location for this pattern
        Modified,                       // Whether the pattern has unsaved data
        EditImage,                      // Image data for editing
    };

    PatternModel(QObject *parent = 0) :
        QAbstractListModel(parent)
    {
    }

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const = 0;
    virtual QVariant data(const QModelIndex &index, int role) const = 0;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const = 0;

    virtual Qt::DropActions supportedDropActions() const = 0;

    virtual bool setData(const QModelIndex &index, const QVariant &value,
                         int role = Qt::EditRole) = 0;

    virtual bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) = 0;
    virtual bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) = 0;

    virtual QUndoStack *getUndoStack() = 0;
};

#endif // PATTERNMODEL
