#ifndef SERIALCOMMANDTESTS_H
#define SERIALCOMMANDTESTS_H

#include <QObject>

class SerialCommandTests : public QObject
{
    Q_OBJECT
private slots:
    void initTest();
    void initMaskedTest();
};

#endif // SERIALCOMMANDTESTS_H
