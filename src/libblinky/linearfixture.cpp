#include "linearfixture.h"

LinearFixture::LinearFixture(unsigned int length, QObject *parent) :
    Fixture(parent)
{
    setLength(length);
}

LinearFixture::~LinearFixture()
{

}

QString LinearFixture::getName() const
{
    return "Linear";
}

unsigned int LinearFixture::getLength() const
{
    return length;
}

void LinearFixture::setLength(unsigned int newLength)
{
    length = newLength;

    extents.setLeft(std::numeric_limits<int>::max());
    extents.setRight(std::numeric_limits<int>::min());
    extents.setTop(std::numeric_limits<int>::max());
    extents.setBottom(std::numeric_limits<int>::min());

    unsigned int x = 0;
    for (unsigned int y = 0; y < newLength; y++) {
        QPoint point(x,y);

        locations.append(point);

        if(extents.left() > point.x())
            extents.setLeft(point.x());
        if(extents.right() < point.x())
            extents.setRight(point.x());
        if(extents.top() > point.y())
            extents.setTop(point.y());
        if(extents.bottom() < point.y())
            extents.setBottom(point.y());
    }
}
