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
        return (Qt::ItemIsEnabled
                | Qt::ItemIsDropEnabled);

    return (Qt::ItemIsEnabled
            | Qt::ItemIsSelectable
            | Qt::ItemIsEditable
            | Qt::ItemIsDragEnabled);
}

Qt::DropActions PatternCollectionModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

void PatternCollectionModel::connectPattern(QPointer<Pattern> pattern)
{
    connect(pattern->getModel(), &PatternModel::dataChanged,
            this, &PatternCollectionModel::on_patternDataChanged);
}

void PatternCollectionModel::disconnectPattern(QPointer<Pattern> pattern)
{
    disconnect(pattern->getModel(), &PatternModel::dataChanged,
               this, &PatternCollectionModel::on_patternDataChanged);
}

void PatternCollectionModel::on_patternDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);

    // TODO: filter any events here?

    // If the first row was modified, emit a data changed notification so the views will know to redraw.
    if (topLeft.row() == 0) {
        QVector<int> emitRoles;
        emitRoles.append(Qt::DisplayRole);
        emit(dataChanged(index(0), index(patterns.count()-1), emitRoles));
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

//    if (role == Qt::ToolTipRole)
//        return patterns.at(index.row())->getName();

    if (role == PreviewImage)
        return patterns.at(index.row())->getEditImage(0);

    return QVariant();
}

bool PatternCollectionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

// pushUndoState();

    if (role == PatternPointer) {
        // disconnectPattern(patterns.at(index.row()));
        patterns.replace(index.row(), qvariant_cast<QPointer<Pattern> >(value));
        connectPattern(patterns.at(index.row()));

        QVector<int> roles;
        roles.append(role);
        emit dataChanged(index, index, roles);
        return true;
    }

    if (role == Qt::EditRole) {
        // Find the UUID
        QPointer<Pattern> source;
        for (QPointer<Pattern> pattern : patterns) {
            if (pattern->getUuid() == value.toUuid()) {
                source = pattern;
                break;
            }
        }

        if (source.isNull())
            return false;

        // disconnectPattern(patterns.at(index.row()));
        patterns.replace(index.row(), source);
        // connectPattern(patterns.at(index.row()));

        QVector<int> roles;
        roles.append(role);
        emit dataChanged(index, index, roles);
        return true;
    }

    return false;
}

bool PatternCollectionModel::insertRows(int position, int rows, const QModelIndex &)
{
// pushUndoState();
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        // TODO: Add the size to PatternCollectionModel model, so we don't have to make fake values here.
        patterns.insert(position, new Pattern(Pattern::Scrolling, QSize(1, 1), 1));
        connectPattern(patterns.at(position));
    }

    endInsertRows();
    return true;
}

bool PatternCollectionModel::removeRows(int position, int rows, const QModelIndex &)
{
// pushUndoState();
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        // disconnectPattern(patterns.at(position));
        patterns.removeAt(position);
    }

    endRemoveRows();
    return true;
}
