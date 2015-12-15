#include "patterncollection.h"

#include <QDebug>

PatternCollection::PatternCollection()
{
}

PatternCollection::~PatternCollection()
{
}

QList<Pattern *> PatternCollection::patterns()
{
    QList<Pattern * > patterns;
    for (int i = 0; i < count(); i++)
        patterns.append(at(i));

    return patterns;
}

void PatternCollection::add(QPointer<Pattern> newPattern, int index)
{
    model.insertRow(index);
    model.setData(model.index(index),
                     // qVariantFromValue((void *) newPattern),
                     QVariant::fromValue(newPattern),
                     PatternCollectionModel::PatternPointer);
}

void PatternCollection::remove(int index)
{
    model.removeRow(index);
}

QPointer<Pattern> PatternCollection::at(int index)
{
    return qvariant_cast<QPointer<Pattern> >(model.index(index).data(PatternCollectionModel::
                                                                        PatternPointer));
}

int PatternCollection::count() const
{
    return model.rowCount();
}

bool PatternCollection::isEmpty() const
{
    return count() == 0;
}
