#ifndef PATTERNCOLLECTION_H
#define PATTERNCOLLECTION_H

#include "libblinkyglobal.h"

#include "patterncollectionmodel.h"

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

private:
    PatternCollectionModel model;
};

#endif // PATTERNCOLLECTION_H
