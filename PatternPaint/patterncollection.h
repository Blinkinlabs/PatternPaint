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

    bool getModified() const;

    /// Initialize the pattern collection from an image file
    /// @param newFile URL of file to load
    bool load(const QString newFileName);

    /// Save the pattern collection to a new file
    /// newFileInfo URL of the new file to save to.
    bool saveAs(const QString newFileName);

private:
    PatternCollectionModel model;
};

#endif // PATTERNCOLLECTION_H
