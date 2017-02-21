#ifndef LINEARFIXTURE_H
#define LINEARFIXTURE_H

#include <QObject>

#include "fixture.h"

class LinearFixture : public Fixture
{
    Q_OBJECT

public:
    LinearFixture(unsigned int length,
                  QObject *parent = 0);
    ~LinearFixture();

    unsigned int getLength() const;
    void setLength(unsigned int newLength);

private:
    unsigned int length;
};


#endif // LINEARFIXTURE_H
