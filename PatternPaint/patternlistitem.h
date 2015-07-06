#ifndef PATTERNLISTITEM_H
#define PATTERNLISTITEM_H

#include "QListWidgetItem"
#include "QPointer"
#include "QImage"


class PatternListItem : public QListWidgetItem
{
public:
    PatternListItem();
    ~PatternListItem();

    void updatePattern(const QImage& newPattern);

private:
    QImage pattern;       /// QImage representation of the pattern
};

#endif // PATTERNLISTITEM_H
