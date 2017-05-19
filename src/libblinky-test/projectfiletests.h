#ifndef PROJECTFILETESTS_H
#define PROJECTFILETESTS_H

#include "projectfile.h"
#include "exponentialbrightness.h"
#include "pattern.h"

#include <QObject>

class ProjectFileTests : public QObject
{
    Q_OBJECT
private slots:
    void headerVersionTest();

    void sceneConfigurationTest();

    void patternsTest();

private:

    QPointer<Fixture> fixture;

};



#endif // PROJECTFILETESTS_H
