#include "projectfile.h"

#include <QDebug>
#include <QSettings>

ProjectFile::ProjectFile()
{

}


bool ProjectFile::save(QString filename, QPointer<Fixture> fixture, PatternCollection *newPatterncollection)
{
    QSettings settings;

    qDebug() << "Save project:" << filename;

    // Create a new file
    QFile file(filename);
    file.open(QIODevice::WriteOnly);


    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_8);

    // write Header
    out << (QString)PROJECT_HEADER;
    out << (float)PROJECT_FORMAT_VERSION;
    out << (QString)GIT_VERSION;

    // write scene configuration
    out << (QString)settings.value("BlinkyTape/firmwareName", BLINKYTAPE_DEFAULT_FIRMWARE_NAME).toString();
    out << (QString)fixture->getName();
    out << (qint32)fixture->getExtents().width();
    out << (qint32)fixture->getExtents().height();
    out << (qint32)fixture->getColorMode();

    //writePatterns(out);

    for(int i=0;i<newPatterncollection->count();i++){

        out << (qint32)newPatterncollection->at(i)->getType();
        newPatterncollection->at(i)->getModel()->writeDataToStream(out);

        newPatterncollection->at(i)->setModified(false);
        newPatterncollection->at(i)->getModified();

    }

    file.close();

    return true;
}


bool ProjectFile::open(QString filename, SceneTemplate* newScenetemplate, PatternCollection *newPatterncollection)
{
    qDebug()<<"Open Project:" << filename;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)){
        qDebug()<< "Error: Cannot read project file ";
        return false;
    }

    // read project file
    QString header;
    QString patternPaintVersion;
    float formatVersion;
    qint32 width;
    qint32 height;
    qint32 colorMode;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_8);

    // read Header
    in >> header;
    if(header != PROJECT_HEADER){
        qDebug()<< "Error: Header incorrectly";
        return false;
    }

    in >> formatVersion;
    if(formatVersion != PROJECT_FORMAT_VERSION){
        qDebug()<< "Error: Format version incorrectly";
        return false;
    }

    in >> patternPaintVersion;

    qDebug() << "Project created with Pattern Paint version:" << patternPaintVersion;


    // read scene configuration
    in >> newScenetemplate->firmwareName;
    in >> newScenetemplate->fixtureType;
    in >> width;
    in >> height;
    in >> colorMode;

    newScenetemplate->colorMode = (ColorMode)colorMode;
    newScenetemplate->size = QSize(width,height);

    if(in.status() != QDataStream::Ok)
        return false;


    // read Patterns
    if(readPatterns(in, newPatterncollection) && in.status() == QDataStream::Ok){
        qDebug() << "Project successful readed";
    }else{
        qDebug() << "Project read failed!";
        return false;
    }

    return true;
}


bool ProjectFile::readPatterns(QDataStream& stream, PatternCollection *newPatterncollection)
{

    newPatterncollection->clear();

    while(!stream.atEnd()) {
            qint32 type;
            stream >> type;

            if(type == Pattern::Scrolling)
            {

                Pattern *pattern = new Pattern(Pattern::Scrolling,
                                                           QSize(1,1),0);
                if(!pattern->getModel()->readDataFromStream(stream))
                    return false;
                newPatterncollection->add(pattern,newPatterncollection->count());
            }
            else if(type == Pattern::FrameBased)
            {
                Pattern *pattern = new Pattern(Pattern::FrameBased,
                                                           QSize(1,1),0);
                if(!pattern->getModel()->readDataFromStream(stream))
                    return false;
                newPatterncollection->add(pattern,newPatterncollection->count());
            }
            else
            {
                qDebug() << "Invalid data section type!";
                return false;
            }
    }

    return true;

}
