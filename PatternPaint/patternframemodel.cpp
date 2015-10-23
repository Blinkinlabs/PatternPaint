#include "patternframemodel.h"
#include "undocommand.h"
#include <QColor>
#include <QDebug>
#include <QPainter>

#define FRAME_COLOR_DEFAULT    QColor(0,0,0,255)

PatternFrameModel::PatternFrameModel(QSize size, QObject *parent) :
    QAbstractListModel(parent),
    size(size)
{
    undoStack.setUndoLimit(50);
}

int PatternFrameModel::rowCount(const QModelIndex &) const
{
    return frames.count();
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
    undoStack.push(new UndoCommand(this, frames, size));

    if(modified != true) {
        modified = true;

        QVector<int> roles;
        roles.push_back(Modified);
        emit dataChanged(this->index(0), this->index(rowCount()), roles);
    }
}

void PatternFrameModel::applyUndoState(QList<QImage> &newFrames, QSize newSize)
{
    // TODO: Handle the whole state, not just the frames...
    frames = newFrames;
    size = newSize;

    modified = true;

    QVector<int> roles;
    roles.push_back(FrameSize);
    roles.push_back(FrameData);
    roles.push_back(Modified);
    emit dataChanged(this->index(0), this->index(rowCount()), roles);
}

QVariant PatternFrameModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= frames.count() || index.row() < 0)
        return QVariant();
    if (role == FrameData || role == Qt::EditRole)
        return frames.at(index.row());
    else if (role == FrameSize)
        return size;
    else if (role == FileName)
        return fileInfo;
    else if (role == Modified)
        return modified;
    else
        return QVariant();
}

bool PatternFrameModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    pushUndoState();

    if(role == FrameData || role == Qt::EditRole) {
        //TODO: enforce size scaling here?
        frames.replace(index.row(), value.value<QImage>());
        QVector<int> roles;
        roles.push_back(FrameData);
        emit dataChanged(index, index, roles);
        return true;
    }
    else if(role == FrameSize) {
        for(int row = 0; row < rowCount(); row++) {
            QImage newImage;
            bool scale = true;      // Enforce scaling...

            if(scale) {
                newImage = frames.at(row).scaled(value.toSize());
            }
            else {
                newImage = QImage(value.toSize(),
                               QImage::Format_ARGB32_Premultiplied);
                newImage.fill(FRAME_COLOR_DEFAULT);

                QPainter painter(&newImage);
                painter.drawImage(0,0,frames.at(row));
            }

            frames.replace(row,newImage);
        }

        QVector<int> roles;
        roles.push_back(FrameSize);
        roles.push_back(FrameData);

        emit dataChanged(this->index(0), this->index(rowCount()), roles);
        return true;
    }
    else if(role == FileName) {
        fileInfo = value.toString();

        QVector<int> roles;
        roles.push_back(FileName);
        emit dataChanged(this->index(0), this->index(rowCount()), roles);
    }
    else if(role == Modified) {
        modified = value.toBool();

        QVector<int> roles;
        roles.push_back(Modified);
        emit dataChanged(this->index(0), this->index(rowCount()), roles);
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

    // TODO: what does 'data changed' mean in these circumstances?
    QVector<int> roles;
    roles.push_back(FrameData);
    emit dataChanged(this->index(0), this->index(rowCount()), roles);
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

    // TODO: what does 'data changed' mean in these circumstances?
    QVector<int> roles;
    roles.push_back(FrameData);
    emit dataChanged(this->index(0), this->index(rowCount()), roles);
    return true;
}
