#ifndef PATTERNCOLLECTION_H
#define PATTERNCOLLECTION_H

#include "patterncollectionmodel.h"

class PatternCollection
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

    bool writePatterns(QDataStream& stream);

    bool readPatterns(QDataStream& stream);

private:
    PatternCollectionModel model;
};

#endif // PATTERNCOLLECTION_H
