#include "linearfixture.h"

LinearFixture::LinearFixture(unsigned int length, QObject *parent) :
    Fixture(parent)
{
    setName("Linear");

    setLength(length);
}

unsigned int LinearFixture::getLength() const
{
    return length;
}

void LinearFixture::setLength(unsigned int newLength)
{
    length = newLength;

    QList<QPoint> newLocations;

    unsigned int x = 0;
    for (unsigned int y = 0; y < newLength; y++) {
        newLocations.append(QPoint(x,y));
    }

    setLocations(newLocations);
}
