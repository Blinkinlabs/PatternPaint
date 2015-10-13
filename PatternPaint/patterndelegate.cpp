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

//    // Show modified indicator
//    if (index.data(PatternItem::Modified).toBool()) {
//        painter->drawImage(QRect(option.rect.x() + 2, option.rect.y() + 2, 16, 16), QImage(":/resources/images/modified.png"));
//    }

//    // Draw index
//    if (index.data(Qt::DisplayRole).toInt() != -1) {
//        painter->setPen(QColor::fromRgb(255,255,0));
//        painter->drawText(option.rect.x(), option.rect.y() + scaled.height() + ITEM_LETTERBOX - 10,
//                          QString::number(index.data(Qt::DisplayRole).toInt()));
//    }

    painter->restore();
}

QSize PatternDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex & ) const {
    return QSize(ITEM_WIDTH, ITEM_HEIGHT);
}

