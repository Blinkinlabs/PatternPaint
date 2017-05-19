#include <QBuffer>
#include <QTest>
#include <QDebug>

#include "projectfiletests.h"


void ProjectFileTests::headerVersionTest()
{

    ProjectFile newProjectFile;

    // Pack the data into a stream
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);

    QDataStream stream;
    stream.setDevice(&buffer);

    // write header
    newProjectFile.writeHeaderVersion(stream, PROJECT_FORMAT_VERSION);

    // reset
    buffer.reset();

    // read header
    QVERIFY(newProjectFile.readHeaderVersion(stream) == PROJECT_FORMAT_VERSION);


}

void ProjectFileTests::sceneConfigurationTest()
{

    ProjectFile newProjectFile;
    SceneTemplate newScenetemplate;


    // Pack the data into a stream
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);

    QDataStream stream;
    stream.setDevice(&buffer);


    // write scene configuration
    fixture = Fixture::makeFixture("Linear", QSize(1,1));

    fixture->setColorMode((ColorMode)RGB);


    newProjectFile.writeSceneConfiguration(stream, fixture);

    // reset
    buffer.reset();


    // read scene configuration
    QVERIFY(newProjectFile.readSceneConfiguration(stream, &newScenetemplate) == true);

    QVERIFY(newScenetemplate.size == QSize(1,1));

    QVERIFY(newScenetemplate.fixtureType == "Linear");

    QVERIFY(newScenetemplate.colorMode == (ColorMode)RGB);

    QVERIFY(newScenetemplate.firmwareName == "default");

}

void ProjectFileTests::patternsTest()
{

    ProjectFile newProjectFile;
    PatternCollection newPatterncollection;


    // Pack the data into a stream
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);

    QDataStream stream;
    stream.setDevice(&buffer);

    // write patterns
    Pattern::PatternType type = Pattern::Scrolling;
    QSize size = QSize(3,7);
    float frameSpeed = 15.5;

    Pattern *newPattern = new Pattern(type, size, 0);
    newPattern->setFrameSpeed(frameSpeed);
    newPatterncollection.add(newPattern,0);

    newProjectFile.writePatterns(stream, &newPatterncollection);

    // reset
    buffer.reset();
    newPatterncollection.clear();


    // read patterns
    QVERIFY(newProjectFile.readPatterns(stream, &newPatterncollection) == true);

    QVERIFY(newPatterncollection.at(0)->getType() == Pattern::Scrolling);

    QVERIFY(newPatterncollection.at(0)->getFrameSize() == size);

    QVERIFY(newPatterncollection.at(0)->getFrameSpeed() == frameSpeed);

}
