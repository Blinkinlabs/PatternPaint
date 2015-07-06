#include "patternlistitem.h"
#include "QIcon"

PatternListItem::PatternListItem()
{
    updatePattern(QImage(100,100,QImage::Format_ARGB32_Premultiplied));

    this->setText("AAAAA");
}

PatternListItem::~PatternListItem()
{

}

void PatternListItem::updatePattern(const QImage& newPattern)
{
    pattern = newPattern;

//    this->setIcon(QIcon(pattern));
}
