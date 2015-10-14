#include "patterndelegate.h"
#include "pattern.h"

#include <QDebug>

#define ITEM_HEIGHT 120
#define ITEM_WIDTH 150
#define ITEM_LETTERBOX 10

PatternDelegate::PatternDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}

PatternDelegate::~PatternDelegate()
{

}

void PatternDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex & index ) const {
    painter->save();
    QImage img = qvariant_cast<QImage>(index.data(Pattern::PreviewImage));

    QImage scaled = img.scaled(ITEM_WIDTH - 2*ITEM_LETTERBOX, ITEM_HEIGHT - 2*ITEM_LETTERBOX);

    QStyledItemDelegate::paint(painter, option, index);

    // Draw the image
    painter->fillRect(QRect(option.rect.x()+ITEM_LETTERBOX, option.rect.y()+ITEM_LETTERBOX, scaled.width(), scaled.height()), QColor(10,10,10));
    painter->drawImage(option.rect.x()+ITEM_LETTERBOX, option.rect.y()+ITEM_LETTERBOX, scaled);

    painter->restore();
}

QSize PatternDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex & ) const {
    return QSize(ITEM_WIDTH, ITEM_HEIGHT);
}

