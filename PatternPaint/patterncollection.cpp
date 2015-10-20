#include "patterncollection.h"
#include <QDebug>

PatternCollection::PatternCollection()
{

}

PatternCollection::~PatternCollection()
{

}

void PatternCollection::addPattern(Pattern* newPattern)
{
    int index = count();
    patterns.insertRow(index);
    patterns.setData(patterns.index(index),
                     qVariantFromValue((void *) newPattern),
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

Pattern * PatternCollection::getPattern(int index)
{
    return (Pattern *)patterns.index(index).data(PatternCollectionModel::PatternPointer).value<void *>();
}

int PatternCollection::count() const
{
    return patterns.rowCount();
}
