#ifndef AVRUPLOADDATA_H
#define AVRUPLOADDATA_H

#include <QByteArray>
#include <vector>
#include "patternoutput.h"

/// Memory class to
class avrUploadData {
public:
    bool init(std::vector<PatternOutput> patterns);

    QByteArray sketch;
    QByteArray patternData;
    QByteArray patternTable;

    int sketchAddress;
    int patternDataAddress;
    int patternTableAddress;

    QString errorString;
};


#endif // AVRUPLOADDATA_H
