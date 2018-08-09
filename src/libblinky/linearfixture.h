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

    Fixture::Type getType() const override;

    unsigned int getLength() const;
    void setLength(unsigned int newLength);

    friend QDataStream &operator<<(QDataStream &out, const LinearFixture &fixture);
    friend QDataStream &operator>>(QDataStream &in, LinearFixture &fixture);

private:
    unsigned int length;
};

LIBBLINKY_EXPORT QDataStream &operator<<(QDataStream &out, const LinearFixture &fixture);
LIBBLINKY_EXPORT QDataStream &operator>>(QDataStream &in, LinearFixture &fixture);


#endif // LINEARFIXTURE_H
