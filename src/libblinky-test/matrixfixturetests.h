#ifndef MATRIXFIXTURETESTS_H
#define MATRIXFIXTURETESTS_H

#include <QObject>

class MatrixFixtureTests : public QObject
{
    Q_OBJECT
private slots:
    void constructorTest();

    void setSizeTest_data();
    void setSizeTest();

    void setModeZigZagTest_data();
    void setModeZigZagTest();

    void setModeRowsTest_data();
    void setModeRowsTest();

    void setModeColsTest_data();
    void setModeColsTest();
};

#endif // MATRIXFIXTURETESTS_H
