#include "outputpreview.h"

#include <QPainter>

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

    outputLocations = fixture->getOutputLocations();
    colorStream = fixture->getColorStreamForFrame(data);

    update();
}

void OutputPreview::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.fillRect(0, 0, this->width(), this->height(), QColor(100, 100, 100, 255));

    if(outputLocations.count() != colorStream.count())
        return;

    for(int i = 0; i < outputLocations.count(); i++) {
//        painter.setPen(colorStream.at(i));
        painter.fillRect(QRect(outputLocations.at(i).x()*5,outputLocations.at(i).y()*5,
                               3,3),
                         colorStream.at(i));
    }

//    // Draw the image and tool preview
//    painter.drawImage(QRect(0, 0,
//                            preview.width(),
//                            preview.height()),
//                      preview);
}

