#include "outputpreview.h"

#include <QPainter>


#define PIXEL_WIDTH .7

OutputPreview::OutputPreview(QWidget *parent) : QWidget(parent)
{

}

OutputPreview::~OutputPreview()
{

}

void OutputPreview::setFixture(Fixture *newFixture)
{
    fixture = newFixture;
}

void OutputPreview::setFrameData(int, const QImage data)
{
    if(fixture.isNull())
        return;

    extents = fixture->getExtents();
    outputLocations = fixture->getOutputLocations();
    colorStream = fixture->getColorStreamForFrame(data);

    update();
}

void OutputPreview::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(0, 0, this->width(), this->height(), QColor(100, 100, 100, 255));

    qreal width = extents.right()-extents.left() + 1 + PIXEL_WIDTH;
    qreal height = extents.bottom()-extents.top() + 1 + PIXEL_WIDTH;

    qreal scale = this->geometry().width()/width;

    if(scale > this->geometry().height()/height) {
        scale = this->geometry().height()/height;
    }

    painter.setViewTransformEnabled(true);
    painter.scale(scale,scale);

    if(outputLocations.count() != colorStream.count())
        return;

    for(int i = 0; i < outputLocations.count(); i++) {
        painter.fillRect(QRectF(outputLocations.at(i).x()+.5,outputLocations.at(i).y()+.5,
                               PIXEL_WIDTH, PIXEL_WIDTH),
                         colorStream.at(i));
    }
}
