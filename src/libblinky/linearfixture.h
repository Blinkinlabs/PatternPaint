#ifndef LINEARFIXTURE_H
#define LINEARFIXTURE_H

#include "libblinkyglobal.h"

#include <QObject>

#include "fixture.h"

class LIBBLINKY_EXPORT LinearFixture : public Fixture
{
    Q_OBJECT

public:
    LinearFixture(unsigned int length,
                  QObject *parent = 0);

    unsigned int getLength() const;
    void setLength(unsigned int newLength);

private:
    unsigned int length;
};


#endif // LINEARFIXTURE_H
