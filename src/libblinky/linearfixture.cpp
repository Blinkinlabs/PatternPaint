#include "linearfixture.h"

LinearFixture::LinearFixture(unsigned int length, QObject *parent) :
    Fixture(parent)
{
    setName("Linear");

    setLength(length);
}

Fixture::Type LinearFixture::getType() const
{
    return Fixture::Type::LINEAR;
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

QDataStream &operator<<(QDataStream &out, const LinearFixture &fixture)
{
    out << (qint32)1;   // Current version

    out << fixture.getName();
    out << fixture.getColorMode();
    //out << fixture.brightnessModel;
    out << fixture.length;

    return out;
}

QDataStream &operator>>(QDataStream &in, LinearFixture &fixture)
{
    qint32 version;
    in >> version;
    if(version == 1) {
        QString name;
        in >> name;
        fixture.setName(name);

        ColorMode colorMode;
        in >> colorMode;
        fixture.setColorMode(colorMode);

        //in >> fixture.brightnessModel;

        unsigned int length;
        in >> length;

        fixture.setLength(length);
    }
    else {
        // Mark the data stream as corrupted, so that the array read will
        // unwind correctly.
        in.setStatus(QDataStream::ReadCorruptData);
    }

    return in;
}
