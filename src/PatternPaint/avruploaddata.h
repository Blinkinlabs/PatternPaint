#ifndef AVRUPLOADDATA_H
#define AVRUPLOADDATA_H

#include <QByteArray>
#include <vector>
#include "pattern.h"

/// Memory class to
class avrUploadData {
public:
    bool init(std::vector<Pattern> patterns);

    QByteArray sketch;
    QByteArray patternData;
    QByteArray patternTable;

    int sketchAddress;
    int patternDataAddress;
    int patternTableAddress;

    QString errorString;
};


#endif // AVRUPLOADDATA_H
