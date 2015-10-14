#include "patternframedelegate.h"
#include "patternframemodel.h"

#include <QDebug>

#define ITEM_LETTERBOX 5

#define COLOR_BACKGROUND_UNCHECKED    QColor(50,50,50,255)
#define COLOR_BACKGROUND_CHECKED    QColor(0,0,255,255)

PatternFrameDelegate::PatternFrameDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}

QSize PatternFrameDelegate::getScaledImageSize(int height, QSize imageSize) const {
    int width = (height*imageSize.width())/imageSize.height();
    return QSize(width - ITEM_LETTERBOX*2, height - ITEM_LETTERBOX*2);
}

QSize PatternFrameDelegate::getScaledWidgetSize(int height, QSize imageSize) const {
    int width = (height*imageSize.width())/imageSize.height();
    return QSize(width, height);
}

void PatternFrameDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex & index ) const {
    painter->save();

    QImage image = index.data(PatternFrameModel::FrameData).value<QImage>();
    QImage scaled = image.scaled(getScaledImageSize(option.rect.height(), image.size()), Qt::IgnoreAspectRatio);

    QColor backgroundColor = COLOR_BACKGROUND_UNCHECKED;
    if(option.state & QStyle::State_Selected) {
        backgroundColor = COLOR_BACKGROUND_CHECKED;
    }

    // Draw the surrounding box
    painter->fillRect(option.rect, backgroundColor);

    // and the image
    painter->drawImage(option.rect.x()+ITEM_LETTERBOX, option.rect.y()+ITEM_LETTERBOX, scaled);

    painter->restore();
}

QSize PatternFrameDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex & modelIndex) const {
    return getScaledWidgetSize(option.rect.height()+option.decorationSize.height(),
                               modelIndex.data(PatternFrameModel::FrameData).value<QImage>().size());
}

