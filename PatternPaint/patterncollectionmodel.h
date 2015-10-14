#ifndef PATTERNCOLLECTIONMODEL_H
#define PATTERNCOLLECTIONMODEL_H

#include <QAbstractListModel>
#include <QObject>

class PatternCollectionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    PatternCollectionModel(QObject *parent = 0) :
        QAbstractListModel(parent) {}

    /// True if there is a valid pattern selected
    bool hasPattern() const;
};

#endif // PATTERNCOLLECTIONMODEL_H
