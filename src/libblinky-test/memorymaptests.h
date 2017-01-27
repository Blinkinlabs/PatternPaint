#ifndef MEMORYMAPTESTS_H
#define MEMORYMAPTESTS_H

#include <QObject>

class MemoryMapTests : public QObject
{
    Q_OBJECT
private slots:
    void constructorTest();

    void addSectionTest_data();
    void addSectionTest();
};

#endif // MEMORYMAPTESTS_H
