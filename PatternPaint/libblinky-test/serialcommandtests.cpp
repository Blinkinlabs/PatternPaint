#include <QTest>

#include "serialcommandtests.h"
#include "serialcommand.h"

void SerialCommandTests::initTest()
{
    QString name = "commandName";
    QByteArray data(12, 'x');
    QByteArray expectedResponse(34, 'y');
    int timeout = 12;

    SerialCommand testCommand(name,  data, expectedResponse, timeout);
    QVERIFY(name == testCommand.name);
    QVERIFY(data == testCommand.data);
    QVERIFY(expectedResponse == testCommand.expectedResponse);
    QVERIFY(timeout == testCommand.timeout);
}

void SerialCommandTests::initMaskedTest()
{
    QString name = "commandName";
    QByteArray data(12, 'x');
    QByteArray expectedResponse(34, 'y');
    QByteArray expectedResponseMask(34, 'z');
    int timeout = 12;

    SerialCommand testCommand(name,  data, expectedResponse, expectedResponseMask,
                              timeout);
    QVERIFY(name == testCommand.name);
    QVERIFY(data == testCommand.data);
    QVERIFY(expectedResponse == testCommand.expectedResponse);
    QVERIFY(expectedResponseMask == testCommand.expectedResponseMask);
    QVERIFY(timeout == testCommand.timeout);
}
