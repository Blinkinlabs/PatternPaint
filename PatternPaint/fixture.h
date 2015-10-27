#ifndef FIXTURE_H
#define FIXTURE_H

#include <QList>
#include <QColor>
#include "pattern.h"

class Fixture
{
public:
    virtual QList<QColor> getColorStreamForFrame(const QImage frame) = 0;
    virtual int getLedCount() = 0;
};

class MatrixFixture : public Fixture
{
public:
    MatrixFixture(QSize size);

    QList<QColor> getColorStreamForFrame(const QImage frame);
    int getLedCount();

private:
    QSize size;
};

#endif // FIXTURE_H
