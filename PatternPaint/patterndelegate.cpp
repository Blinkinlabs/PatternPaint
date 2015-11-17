#include "patterndelegate.h"

#include "patternframemodel.h"
#include "colors.h"


#include <QDebug>

#define ITEM_LETTERBOX 2
#define FRAME_SIZE 1

#define MINIMUM_WIDTH 2

PatternDelegate::PatternDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

QSize PatternDelegate::getScaledImageSize(int height, QSize imageSize) const {
    int width = (height*imageSize.width())/imageSize.height();

    if(width < ITEM_LETTERBOX*2+MINIMUM_WIDTH)
        width = ITEM_LETTERBOX*2+MINIMUM_WIDTH;

    return QSize(width - ITEM_LETTERBOX*2, height - ITEM_LETTERBOX*2);
}

QSize PatternDelegate::getScaledWidgetSize(int height, QSize imageSize) const {
    int width = (height*imageSize.width())/imageSize.height();

    if(width < ITEM_LETTERBOX*2+MINIMUM_WIDTH)
        width = ITEM_LETTERBOX*2+MINIMUM_WIDTH;

    return QSize(width, height);
}

void PatternDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex & index ) const {
    painter->save();

    QImage image = index.data(PatternFrameModel::FrameImage).value<QImage>();
    QImage scaled = image.scaled(getScaledImageSize(option.rect.height(), image.size()), Qt::IgnoreAspectRatio);

    QColor backgroudColor = COLOR_BACKGROUND_UNCHECKED;
    if(option.state & QStyle::State_Selected) {
        backgroudColor = COLOR_BACKGROUND_CHECKED;
    }

    painter->fillRect(option.rect, backgroudColor);

//    // Draw the frame
//    painter->fillRect(QRect(option.rect.x()+ITEM_LETTERBOX-FRAME_SIZE,
//                            option.rect.y()+ITEM_LETTERBOX-FRAME_SIZE,
//                            scaled.width()+2*FRAME_SIZE,
//                            scaled.height()+2*FRAME_SIZE), COLOR_FRAME_UNSELECTED);

    // and the image
    painter->drawImage(option.rect.x()+ITEM_LETTERBOX, option.rect.y()+ITEM_LETTERBOX, scaled);

    painter->restore();
}

QSize PatternDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex & modelIndex) const {
    return getScaledWidgetSize(option.rect.height()+option.decorationSize.height(),
                               modelIndex.data(PatternFrameModel::FrameImage).value<QImage>().size());
}

