#ifndef PATTERNCOLLECTION_H
#define PATTERNCOLLECTION_H

#include "patterncollectionmodel.h"

class PatternCollection
{
public:
    PatternCollection();
    ~PatternCollection();

    PatternCollectionModel* getModel() {return &patterns;}

    void addPattern(Pattern * newPattern);

    void removePattern(int index);

    Pattern * getPattern(int index);

    int count() const;
    bool hasPattern() const;

private:
    PatternCollectionModel patterns;
};

#endif // PATTERNCOLLECTION_H
