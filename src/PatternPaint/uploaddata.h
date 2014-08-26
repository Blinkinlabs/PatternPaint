#ifndef UPLOADDATA_H
#define UPLOADDATA_H

#include <QByteArray>
#include <vector>
#include "pattern.h"

/// Memory class to
class uploadData {
public:
    bool init(std::vector<Pattern> patterns);

    QByteArray sketch;
    QByteArray patternData;
    QByteArray patternTable;

    int sketchAddess;
    int patternDataAddress;
    int patternTableAddress;

    QString errorString;
};


#endif // UPLOADDATA_H
