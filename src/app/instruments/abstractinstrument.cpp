#include "abstractinstrument.h"

#include "frameeditor.h"
#include "patternframeundocommand.h"

AbstractInstrument::AbstractInstrument(const QString &resource, QObject *parent) :
    QObject(parent),
    mpm(resource)
{
    mcur = QCursor(mpm);
}

bool AbstractInstrument::hasPreview() const
{
    return drawing;
}

const QImage &AbstractInstrument::getPreview() const
{
    return toolPreview;
}


QCursor AbstractInstrument::cursor() const
{
    return mcur;
}
