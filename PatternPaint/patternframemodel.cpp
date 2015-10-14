#include "patternframemodel.h"
#include "undocommand.h"
#include <QColor>
#include <QDebug>
#include <QPainter>

#define FRAME_COLOR_DEFAULT    QColor(0,0,0,255)

PatternFrameModel::PatternFrameModel(QSize size, QObject *parent) :
    QAbstractListModel(parent),
    size(size),
    pushUndo(true)
{
    undoStack.setUndoLimit(50);
}

int PatternFrameModel::rowCount(const QModelIndex &) const
{
    return frames.count();
}

QVariant PatternFrameModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= frames.count() || index.row() < 0)
        return QVariant();

    if (role == FrameData || role == Qt::EditRole)
        return frames.at(index.row());
    else
        return QVariant();
}

Qt::ItemFlags PatternFrameModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled ;
}

Qt::DropActions PatternFrameModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

void PatternFrameModel::pushUndoState()
{
    if(pushUndo)
        undoStack.push(new UndoCommand(this, frames, size));
}

bool PatternFrameModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    pushUndoState();

    if(role == Qt::EditRole || role == PatternFrameModel::FrameData) {
        frames.replace(index.row(), value.value<QImage>());
        QVector<int> roles;
        roles.push_back(role);
        emit dataChanged(index, index, roles);
        return true;
    }

    return false;
}

bool PatternFrameModel::insertRows(int position, int rows, const QModelIndex &)
{
    pushUndoState();
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        QImage newImage(size, QImage::Format_ARGB32_Premultiplied);
        newImage.fill(FRAME_COLOR_DEFAULT);
        frames.insert(position, newImage);
    }

    endInsertRows();
    return true;
}

bool PatternFrameModel::removeRows(int position, int rows, const QModelIndex &)
{
    pushUndoState();
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        frames.removeAt(position);
    }

    endRemoveRows();
    return true;
}

void PatternFrameModel::applyUndoState(QList<QImage> &newFrames, QSize newSize)
{
    // TODO: Handle the whole state, not just the frames...
    frames = newFrames;
    size = newSize;
    emit dataChanged(createIndex(0,0), createIndex(frames.size()-1,0));
}

void PatternFrameModel::resize(QSize newSize, bool scale) {
    size = newSize;

    pushUndoState();
    setPushUndo(false);
    for(int row = 0; row < rowCount(); row++) {
        QImage newImage;
        if(scale) {
            newImage = frames.at(row).scaled(size);
        }
        else {
            newImage = QImage(size,
                           QImage::Format_ARGB32_Premultiplied);
            newImage.fill(FRAME_COLOR_DEFAULT);

            QPainter painter(&newImage);
            painter.drawImage(0,0,frames.at(row));
        }

        setData(createIndex(row,0),newImage);
    }
    setPushUndo(true);
}
