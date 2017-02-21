#ifndef FIXTURETESTS_H
#define FIXTURETESTS_H

#include <QObject>

class FixtureTests : public QObject
{
    Q_OBJECT
private slots:
    void setNameTest();
    void setColorModeTest();
    void setBrightnessModelTest();
    void setLocationsTest();

    void getExtentsTest_data();
    void getExtentsTest();

    void getColorStreamNullBrightnessModelTest();
    void getColorStreamNullFrameTest();
    void getColorStreamBadLocationsTest();
    void getColorStreamBrightnessModelAppliedTest();
};

#endif // FIXTURETESTS_H
