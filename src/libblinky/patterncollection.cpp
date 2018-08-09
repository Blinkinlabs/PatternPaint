#include "patterncollection.h"

#include <QDebug>

PatternCollection::PatternCollection()
{
}

PatternCollection::~PatternCollection()
{
}

PatternCollectionModel *PatternCollection::getModel()
{
    return &model;
}

QList<Pattern *> PatternCollection::patterns() const
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
                     QVariant::fromValue(newPattern),
                     PatternCollectionModel::PatternPointer);
}

void PatternCollection::remove(int index)
{
    model.removeRow(index);
}

void PatternCollection::clear()
{
    model.removeRows(0,count());
}

QPointer<Pattern> PatternCollection::at(int index) const
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

QDataStream &operator<<(QDataStream &out, const PatternCollection &patternCollection)
{
    out << (qint32)1;

    out << patternCollection.model;

    return out;
}

QDataStream &operator>>(QDataStream &in, PatternCollection &patternCollection)
{
    qint32 version;
    in >> version;

    if(version == 1) {
        in >> patternCollection.model;
    }
    else {
        // Mark the data stream as corrupted, so that the array read will
        // unwind correctly.
        in.setStatus(QDataStream::ReadCorruptData);
    }

    return in;
}
