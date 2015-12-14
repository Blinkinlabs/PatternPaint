#include "abstractinstrument.h"

#include "frameeditor.h"
#include "patternframeundocommand.h"

AbstractInstrument::AbstractInstrument(QObject *parent) :
    QObject(parent)
{
}

CustomCursorInstrument::CustomCursorInstrument(const QString &resource, QObject *parent) :
    AbstractInstrument(parent),
    mpm(resource)
{
    mcur = QCursor(mpm);
}

QCursor CustomCursorInstrument::cursor() const
{
    return mcur;
}
