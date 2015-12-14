#include "letterboxscrollarea.h"

LetterboxScrollArea::LetterboxScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
}

void LetterboxScrollArea::resizeEvent(QResizeEvent *event)
{
    float widgetAspect = float(widget()->baseSize().width())
                         /widget()->baseSize().height();

    float widgetWidth = height()*widgetAspect;

    widget()->setMinimumWidth(widgetWidth);
    widget()->setMaximumWidth(widgetWidth);

    QScrollArea::resizeEvent(event);
}
