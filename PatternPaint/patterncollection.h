#ifndef PATTERNCOLLECTION_H
#define PATTERNCOLLECTION_H

#include "patterncollectionmodel.h"

class PatternCollection
{
public:
    PatternCollection();
    ~PatternCollection();

    PatternCollectionModel *getModel()
    {
        return &model;
    }

    void add(QPointer<Pattern> newPattern, int index);

    void remove(int index);

    QPointer<Pattern> at(int index);

    QList<Pattern*> patterns();

    int count() const;

    bool isEmpty() const;

private:
    PatternCollectionModel model;
};

#endif // PATTERNCOLLECTION_H
