#include "abstractinstrument.h"

#include "frameeditor.h"
#include "patternframeundocommand.h"

AbstractInstrument::AbstractInstrument(const QString &resource,
                                       InstrumentConfiguration* instrumentConfiguration,
                                       QObject *parent) :
    QObject(parent),
    instrumentConfiguration(instrumentConfiguration),
    icon(resource)
{
    cursor = QCursor(icon);
}

bool AbstractInstrument::hasPreview() const
{
    return drawing;
}

const QImage &AbstractInstrument::getPreview() const
{
    return preview;
}


const QCursor & AbstractInstrument::getCursor() const
{
    return cursor;
}
