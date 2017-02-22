#ifndef SERIALCOMMANDTESTS_H
#define SERIALCOMMANDTESTS_H

#include <QObject>

class SerialCommandTests : public QObject
{
    Q_OBJECT
private slots:
    void initTest();
    void initMaskedTest();

    void compareNoMaskTest_data();
    void compareNoMaskTest();

    void compareMaskTest_data();
    void compareMaskTest();

    void equalsTest();
    void equalsOperatorNameMismatchTest();
    void equalsOperatorDataMismatchTest();
    void equalsOperatorExpectedResponseMismatchTest();
    void equalsOperatorExpectedResponseMaskMismatchTest();
    void equalsOperatorTimeoutMismatchTest();
};

#endif // SERIALCOMMANDTESTS_H
