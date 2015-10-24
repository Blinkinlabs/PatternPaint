#include "patterncollection.h"
#include <QDebug>

PatternCollection::PatternCollection()
{

}

PatternCollection::~PatternCollection()
{

}

void PatternCollection::addPattern(QPointer<Pattern> newPattern)
{
    int index = count();
    patterns.insertRow(index);
    patterns.setData(patterns.index(index),
                     //qVariantFromValue((void *) newPattern),
                     QVariant::fromValue(newPattern),
                     PatternCollectionModel::PatternPointer);
}


void PatternCollection::removePattern(int index)
{
    patterns.removeRow(index);
}

bool PatternCollection::hasPattern() const
{
    return count() > 0;
}

QPointer<Pattern> PatternCollection::getPattern(int index)
{
    return qvariant_cast<QPointer<Pattern> >(patterns.index(index).data(PatternCollectionModel::PatternPointer));
}

int PatternCollection::count() const
{
    return patterns.rowCount();
}
