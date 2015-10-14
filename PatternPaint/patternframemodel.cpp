#include "patternframemodel.h"
#include <QColor>
#include <QDebug>
#include <QPainter>

#define FRAME_COLOR_DEFAULT    QColor(0,0,0,255)

int PatternFrameModel::rowCount(const QModelIndex &) const
{
    return frames.count();
}

QVariant PatternFrameModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= frames.count())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
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

bool PatternFrameModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
    if (index.isValid()) {
        frames.replace(index.row(), value.value<QImage>());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool PatternFrameModel::insertRows(int position, int rows, const QModelIndex &)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);
    qDebug() << "Insert rows called";

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
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    qDebug() << "Remove rows called";

    for (int row = 0; row < rows; ++row) {
        frames.removeAt(position);
    }

    endRemoveRows();
    return true;
}

void PatternFrameModel::resize(QSize newSize, bool scale) {
    size = newSize;

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
        frames.replace(row, newImage);
    }
}
