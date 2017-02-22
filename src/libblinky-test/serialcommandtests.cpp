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

void SerialCommandTests::compareNoMaskTest_data()
{
    QTest::addColumn<QByteArray>("expectedResponse");
    QTest::addColumn<QByteArray>("response");
    QTest::addColumn<CompareResult>("result");

    QByteArray largeDataA;
    QByteArray largeDataB;
    for(int i = 0; i < 1000; i++) {
        largeDataA.append(static_cast<unsigned char>(i & 0xFF));
        largeDataB.append(static_cast<unsigned char>(i*2 & 0xFF));
    }

    QTest::newRow("not enough data, small")   << QByteArray(1,'x') << QByteArray() << CompareResult::RESPONSE_NOT_ENOUGH_DATA;
    QTest::newRow("not enough data, large")   << largeDataA << largeDataB.mid(0, largeDataB.length()-1) << CompareResult::RESPONSE_NOT_ENOUGH_DATA;

    QTest::newRow("too much data, small")   << QByteArray() << QByteArray(1,'x') << CompareResult::RESPONSE_TOO_MUCH_DATA;
    QTest::newRow("too much data, large")   << largeDataA.mid(0, largeDataB.length()-1) << largeDataB << CompareResult::RESPONSE_TOO_MUCH_DATA;

    QTest::newRow("mismatch, small")   << QByteArray(1,'x') << QByteArray(1,'y') << CompareResult::RESPONSE_MISMATCH;
    QTest::newRow("mismatch, large")   << largeDataA << largeDataB << CompareResult::RESPONSE_MISMATCH;

    QTest::newRow("match, 0 length")   << QByteArray() << QByteArray() << CompareResult::RESPONSE_MATCH;
    QTest::newRow("match, small")   << QByteArray(1,'x') << QByteArray(1,'x') << CompareResult::RESPONSE_MATCH;
    QTest::newRow("match, large")   << largeDataA << largeDataA << CompareResult::RESPONSE_MATCH;
}

void SerialCommandTests::compareNoMaskTest()
{
    QFETCH(QByteArray, expectedResponse);
    QFETCH(QByteArray, response);
    QFETCH(CompareResult, result);

    SerialCommand command("",QByteArray(), expectedResponse);

    QVERIFY(command.testResponse(response) == result);
}

void SerialCommandTests::compareMaskTest_data()
{
    QTest::addColumn<QByteArray>("expectedResponse");
    QTest::addColumn<QByteArray>("expectedResponseMask");
    QTest::addColumn<QByteArray>("response");
    QTest::addColumn<CompareResult>("result");

    QByteArray largeDataA;
    QByteArray largeDataB;
    QByteArray largeMask;
    for(int i = 0; i < 1000; i++) {
        // Make every third value unequal, with a mask
        if(i % 3) {
            largeDataA.append(static_cast<unsigned char>(i & 0xFF));
            largeDataB.append(static_cast<unsigned char>((i+1) & 0xFF));
            largeMask.append('\x00');
        }
        else {
            largeDataA.append(static_cast<unsigned char>(i & 0xFF));
            largeDataB.append(static_cast<unsigned char>(i & 0xFF));
            largeMask.append('x');
        }
    }

    QTest::newRow("zero-length mask ignored")   << QByteArray(1,'x') << QByteArray() << QByteArray(1,'y') << CompareResult::RESPONSE_MISMATCH;

    QTest::newRow("too small mask, small")   << QByteArray(2,'x') << QByteArray(1,'\x00') << QByteArray(2,'x') << CompareResult::RESPONSE_INVALID_MASK;
    QTest::newRow("too small mask, big")   << QByteArray(1000,'x') << QByteArray(999,'\x00') << QByteArray(1000,'x') << CompareResult::RESPONSE_INVALID_MASK;

    QTest::newRow("mask, small")   << QByteArray(1,'x') << QByteArray(1,'\x00') << QByteArray(1, 'y') << CompareResult::RESPONSE_MATCH;
    QTest::newRow("mask, large, verify different") << largeDataA << QByteArray() << largeDataB << CompareResult::RESPONSE_MISMATCH;
    QTest::newRow("mask, large") << largeDataA << largeMask << largeDataB << CompareResult::RESPONSE_MATCH;
}


void SerialCommandTests::compareMaskTest()
{
    QFETCH(QByteArray, expectedResponse);
    QFETCH(QByteArray, expectedResponseMask);
    QFETCH(QByteArray, response);
    QFETCH(CompareResult, result);

    SerialCommand command("",QByteArray(), expectedResponse, expectedResponseMask);

    QVERIFY(command.testResponse(response) == result);
}

void SerialCommandTests::equalsTest()
{
    SerialCommand a("name", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'z'), 123);
    SerialCommand b("name", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'z'), 123);

    QVERIFY((a == b));
}

void SerialCommandTests::equalsOperatorNameMismatchTest()
{
    SerialCommand a("name", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'z'), 123);
    SerialCommand b("nameb", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'z'), 123);

    QVERIFY(!(a == b));
}

void SerialCommandTests::equalsOperatorDataMismatchTest()
{
    SerialCommand a("name", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'z'), 123);
    SerialCommand b("name", QByteArray(3,'y'), QByteArray(4, 'y'), QByteArray(5, 'z'), 123);

    QVERIFY(!(a == b));
}

void SerialCommandTests::equalsOperatorExpectedResponseMismatchTest()
{
    SerialCommand a("name", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'z'), 123);
    SerialCommand b("name", QByteArray(3,'x'), QByteArray(4, 'x'), QByteArray(5, 'z'), 123);

    QVERIFY(!(a == b));
}

void SerialCommandTests::equalsOperatorExpectedResponseMaskMismatchTest()
{
    SerialCommand a("name", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'z'), 123);
    SerialCommand b("name", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'a'), 123);

    QVERIFY(!(a == b));
}

void SerialCommandTests::equalsOperatorTimeoutMismatchTest()
{
    SerialCommand a("name", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'z'), 123);
    SerialCommand b("name", QByteArray(3,'x'), QByteArray(4, 'y'), QByteArray(5, 'z'), 124);

    QVERIFY(!(a == b));
}
