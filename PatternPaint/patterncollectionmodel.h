#ifndef PATTERNCOLLECTIONMODEL_H
#define PATTERNCOLLECTIONMODEL_H

#include "pattern.h"

#include <QAbstractListModel>
#include <QObject>
#include <QUndoGroup>

class PatternCollectionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        PreviewImage = Qt::UserRole,
        PatternPointer,
    };

    PatternCollectionModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    Qt::DropActions supportedDropActions() const;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

private:
    QList<Pattern *> patterns;
};

#endif // PATTERNCOLLECTIONMODEL_H
