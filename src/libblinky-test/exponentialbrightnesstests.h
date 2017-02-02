#ifndef EXPONENTIALBRIGHTNESSTESTS_H
#define EXPONENTIALBRIGHTNESSTESTS_H

#include <QObject>

class ExponentialBrightnessTests : public QObject
{
    Q_OBJECT
private slots:
    void constructorTest();
    void constructorZeroTest();

    void correctTest_data();
    void correctTest();
};

#endif // EXPONENTIALBRIGHTNESSTESTS_H
