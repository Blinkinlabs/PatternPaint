#include "patternitemdelegate.h"
#include "patternitem.h"

#include <QDebug>

#define ITEM_HEIGHT 120
#define ITEM_LETTERBOX 10

PatternItemDelegate::PatternItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}

PatternItemDelegate::~PatternItemDelegate()
{

}

void PatternItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex & index ) const {
    qDebug() << "repainting!";

    painter->save();
    QImage img = qvariant_cast<QImage>(index.data(PatternItem::PreviewImage));
    QImage scaled = img.scaledToHeight(ITEM_HEIGHT - 2*ITEM_LETTERBOX);
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

QSize PatternItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const {
    Q_UNUSED(option);
    QSize size = qvariant_cast<QSize>(index.data(PatternItem::PatternSize));
    float aspect = ITEM_HEIGHT/size.height();
    return QSize(size.width()*aspect, ITEM_HEIGHT);
}
