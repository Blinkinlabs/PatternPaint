#ifndef LINEARFIXTURETESTS_H
#define LINEARFIXTURETESTS_H

#include <QObject>

class LinearFixtureTests : public QObject
{
    Q_OBJECT
private slots:
    void constructorTest();

    void setLengthTest_data();
    void setLengthTest();

};

#endif // LINEARFIXTURETESTS_H
