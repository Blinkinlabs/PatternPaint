#ifndef LINEARFIXTURETESTS_H
#define LINEARFIXTURETESTS_H

#include "multitests.h"

#include <QObject>

class LinearFixtureTests : public QObject
{
    Q_OBJECT
private slots:
    void constructorTest();

    void setLengthTest_data();
    void setLengthTest();

};

TEST_DECLARE(LinearFixtureTests)

#endif // LINEARFIXTURETESTS_H
