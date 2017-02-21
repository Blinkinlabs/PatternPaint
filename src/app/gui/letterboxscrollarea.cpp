#include "letterboxscrollarea.h"

#include <QDebug>

LetterboxScrollArea::LetterboxScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
}

void LetterboxScrollArea::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);
}
