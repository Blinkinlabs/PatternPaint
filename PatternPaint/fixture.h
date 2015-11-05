#ifndef FIXTURE_H
#define FIXTURE_H

#include <QObject>
#include <QList>
#include <QColor>
#include "pattern.h"

class Fixture : public QObject
{
    Q_OBJECT

public:
    Fixture(QObject *parent = 0);

    virtual QList<QColor> getColorStreamForFrame(const QImage frame) const = 0;
    virtual int getLedCount() const = 0;

    // TODO: Make these non-generic.
    virtual QSize getSize() const = 0;
    virtual void setSize(QSize newSize) = 0;
};

class MatrixFixture : public Fixture
{
    Q_OBJECT

public:
    MatrixFixture(QSize size, QObject *parent = 0);

    QList<QColor> getColorStreamForFrame(const QImage frame) const;
    int getLedCount() const;
    QSize getSize() const;
    void setSize(QSize newSize);

private:
    QSize size;
};

#endif // FIXTURE_H
