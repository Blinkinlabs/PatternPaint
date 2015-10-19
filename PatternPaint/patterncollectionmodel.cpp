#include "patterncollectionmodel.h"
#include <QDebug>

PatternCollectionModel::PatternCollectionModel(QObject *parent) :
    QAbstractListModel(parent)
{

}


int PatternCollectionModel::rowCount(const QModelIndex &) const
{
    return patterns.count();
}

QVariant PatternCollectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= patterns.count() || index.row() < 0)
        return QVariant();

    if (role == PreviewImage)
        return patterns.at(index.row())->getFrame(0);

    else if (role == PatternPointer)
        return qVariantFromValue((void *) patterns.at(index.row()));

    else
        return QVariant();
}

Qt::ItemFlags PatternCollectionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled ;
}

Qt::DropActions PatternCollectionModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool PatternCollectionModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

//    pushUndoState();

    if(role == PatternPointer) {
        patterns.replace(index.row(), (Pattern *) value.value<void *>());
        QVector<int> roles;
        roles.push_back(role);
        emit dataChanged(index, index, roles);
        return true;
    }
    return false;
}

bool PatternCollectionModel::insertRows(int position, int rows, const QModelIndex &)
{
//    pushUndoState();
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        // TODO: Add the size to PatternCollectionModel model, so we don't have to make fake values here.
        patterns.insert(position, new Pattern(QSize(1,1),1));

        // TODO: connect DataChanged() here so we can redraw ourselves?
    }

    endInsertRows();
    return true;
}

bool PatternCollectionModel::removeRows(int position, int rows, const QModelIndex &)
{
//    pushUndoState();
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        patterns.removeAt(position);
    }

    endRemoveRows();
    return true;
}
