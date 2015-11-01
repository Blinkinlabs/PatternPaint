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

Qt::ItemFlags PatternCollectionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled ;
}

Qt::DropActions PatternCollectionModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

void PatternCollectionModel::connectPattern(QPointer<Pattern> pattern)
{
    connect(pattern->getModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(on_patternDataChanged(QModelIndex,QModelIndex,QVector<int>)));
}

void PatternCollectionModel::disconnectPattern(QPointer<Pattern> pattern)
{
    disconnect(pattern->getModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
               this, SLOT(on_patternDataChanged(QModelIndex,QModelIndex,QVector<int>)));
}


void PatternCollectionModel::on_patternDataChanged(QModelIndex begin, QModelIndex, QVector<int>)
{
    // If the first row was modified, emit a data changed notification so the views will know to redraw.
    if(begin.row() == 0) {
        QVector<int> roles;
        roles.append(Qt::DisplayRole);
        emit(dataChanged(index(0),index(patterns.count()-1),roles));
    }
}


QVariant PatternCollectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= patterns.count() || index.row() < 0)
        return QVariant();

    if (role == PatternPointer)
        return QVariant::fromValue(patterns.at(index.row()));

    if (role == Qt::EditRole)
        return patterns.at(index.row())->getUuid();

//    else if (role == Qt::ToolTipRole)
//        return patterns.at(index.row())->getPatternName();

    else if (role == PreviewImage)
        return patterns.at(index.row())->getEditImage(0);

    return QVariant();
}

bool PatternCollectionModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

//    pushUndoState();

    if(role == PatternPointer) {
        //disconnectPattern(patterns.at(index.row()));
        patterns.replace(index.row(), qvariant_cast<QPointer<Pattern> >(value));
        connectPattern(patterns.at(index.row()));

        QVector<int> roles;
        roles.append(role);
        emit dataChanged(index, index, roles);
        return true;
    }

    else if(role == Qt::EditRole) {
        // Find the UUID
        QPointer<Pattern> source;
        for(int i = 0; i < patterns.count(); i++) {
            if(patterns.at(i)->getUuid() == value.toUuid()) {
                source = patterns.at(i);
                break;
            }
        }

        if(source.isNull())
            return false;

        //disconnectPattern(patterns.at(index.row()));
        patterns.replace(index.row(), source);
        //connectPattern(patterns.at(index.row()));

        QVector<int> roles;
        roles.append(role);
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
        connectPattern(patterns.at(position));
    }

    endInsertRows();
    return true;
}

bool PatternCollectionModel::removeRows(int position, int rows, const QModelIndex &)
{
//    pushUndoState();
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        //disconnectPattern(patterns.at(position));
        patterns.removeAt(position);
    }

    endRemoveRows();
    return true;
}
