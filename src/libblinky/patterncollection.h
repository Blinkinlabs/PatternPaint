#ifndef PATTERNCOLLECTION_H
#define PATTERNCOLLECTION_H

#include "libblinkyglobal.h"
#include "patterncollectionmodel.h"

#include <QDataStream>

class LIBBLINKY_EXPORT PatternCollection
{
public:
    PatternCollection();
    ~PatternCollection();

    PatternCollectionModel *getModel();

    void add(QPointer<Pattern> newPattern, int index);

    void remove(int index);

    void clear();

    QPointer<Pattern> at(int index) const;

    QList<Pattern*> patterns() const;

    int count() const;

    bool isEmpty() const;

    LIBBLINKY_EXPORT friend QDataStream &operator<<(QDataStream &out, const PatternCollection &patternCollection);
    LIBBLINKY_EXPORT friend QDataStream &operator>>(QDataStream &in, PatternCollection &patternCollection);

private:
    PatternCollectionModel model;
};

LIBBLINKY_EXPORT QDataStream &operator<<(QDataStream &out, const PatternCollection &patternCollection);
LIBBLINKY_EXPORT QDataStream &operator>>(QDataStream &in, PatternCollection &patternCollection);

#endif // PATTERNCOLLECTION_H
