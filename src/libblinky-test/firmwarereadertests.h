#ifndef FIRMWAREREADERTEST_H
#define FIRMWAREREADERTEST_H

#include "multitests.h"

#include <QObject>

class FirmwareReaderTests : public QObject
{
    Q_OBJECT
private slots:
    void parseHexLineBadFormatTest_data();
    void parseHexLineBadFormatTest();

    void parseHexLineAddressTest_data();
    void parseHexLineAddressTest();

    void parseHexLineTypeTest_data();
    void parseHexLineTypeTest();

    void parseHexLineDataTest_data();
    void parseHexLineDataTest();
};

TEST_DECLARE(FirmwareReaderTests)

#endif // FIRMWAREREADERTEST_H
