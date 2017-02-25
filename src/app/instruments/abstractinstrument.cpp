#include "abstractinstrument.h"

#include "frameeditor.h"
#include "patternframeundocommand.h"

#include <QPainter>
#include <QDebug>

AbstractInstrument::AbstractInstrument(const QString &resource,
                                       InstrumentConfiguration* instrumentConfiguration,
                                       QObject *parent) :
    QObject(parent),
    instrumentConfiguration(instrumentConfiguration),
    icon(resource)
{
    cursor = QCursor(icon);
}

bool AbstractInstrument::isDrawing() const
{
    return drawing;
}

void AbstractInstrument::updatePreview(const QImage &frameData, QPoint point)
{
    if(preview.size() != frameData.size())
        preview = QImage(frameData.size(),
                         QImage::Format_ARGB32_Premultiplied);
    preview.fill(QColor(0, 0, 0, 0));

    QPainter painter(&preview);
    QColor previewColor;

    if(frameData.pixelColor(point).lightness() < 128) {
        previewColor = QColor(128+20,128+20,128+20,128);
    }
    else {
        previewColor = QColor(128-20,128-20,128-20,128);
    }

    painter.setPen(QPen(previewColor, 1));
    painter.drawPoint(point);
}

const QImage &AbstractInstrument::getPreview() const
{
    return preview;
}


const QCursor & AbstractInstrument::getCursor() const
{
    return cursor;
}
