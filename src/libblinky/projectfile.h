#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "patterncollectionmodel.h"
#include "patterncollection.h"
#include "fixture.h"
#include "firmwarestore.h"
#include "scenetemplate.h"


#define PROJECT_HEADER     "PatternPaint Project"   /// Header of PatternPaint Project
#define PROJECT_FORMAT_VERSION     1.0              /// Version of PatternPaint Project format


class ProjectFile
{
public:
    ProjectFile();

    bool save(QString filename, QPointer<Fixture> fixture, PatternCollection* newPatterncollection);

    bool open(QString filename, SceneTemplate* newScenetemplate, PatternCollection* newPatterncollection);

    void writeHeaderVersion(QDataStream& stream, float version);

    float readHeaderVersion(QDataStream& stream);

    void writeSceneConfiguration(QDataStream &stream, QPointer<Fixture> fixture);

    bool readSceneConfiguration(QDataStream &stream, SceneTemplate* newScenetemplate);

    void writePatterns(QDataStream &stream, PatternCollection* newPatterncollection);

    bool readPatterns(QDataStream& stream, PatternCollection* newPatterncollection);
};

#endif // PROJECTFILE_H
