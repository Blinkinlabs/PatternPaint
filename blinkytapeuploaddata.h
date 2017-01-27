#ifndef AVRUPLOADDATA_H
#define AVRUPLOADDATA_H

#include <QByteArray>
#include <vector>
#include "patternwriter.h"

struct FlashSection {
    FlashSection() {}

    /// Create a new flash section
    /// @param address Address in the flash memory where the data should be stored
    /// @param data Data to store
    FlashSection(QString name, int address, const QByteArray& data) :
        name(name),
        address(address),
        data(data)
    {
    }

    QString name;       /// Section name, for debugging
    int address;        /// Address to store the data
    QByteArray data;    /// Data to store
};


/// Utility class for assembling flash sections for the BlinkyTape
class BlinkyTapeUploadData
{
public:
    bool init(QList<PatternWriter> &patterns);

    FlashSection sketchSection;
    FlashSection patternDataSection;
    FlashSection patternTableSection;

    QString errorString;
};

#endif // AVRUPLOADDATA_H
