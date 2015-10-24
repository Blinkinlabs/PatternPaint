#include "patterncollectiondelegate.h"
#include "patterncollectionmodel.h"

#include <QDebug>

#define ITEM_HEIGHT 120
#define ITEM_WIDTH 150
#define ITEM_LETTERBOX 10

#define COLOR_BACKGROUND_UNCHECKED    QColor(50,50,50,255)
#define COLOR_BACKGROUND_CHECKED    QColor(0,0,255,255)

PatternCollectionDelegate::PatternCollectionDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

void PatternCollectionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex & index ) const {
    painter->save();
    QImage img = qvariant_cast<QImage>(index.data(PatternCollectionModel::PreviewImage));

    QImage scaled = img.scaled(ITEM_WIDTH - 2*ITEM_LETTERBOX, ITEM_HEIGHT - 2*ITEM_LETTERBOX);

//    QStyledItemDelegate::paint(painter, option, index);

    QColor backgroundColor = COLOR_BACKGROUND_UNCHECKED;
    if(option.state & QStyle::State_Selected) {
        backgroundColor = COLOR_BACKGROUND_CHECKED;
    }

    // Draw the surrounding box
    painter->fillRect(option.rect, backgroundColor);

    // Draw the image
    painter->fillRect(QRect(option.rect.x()+ITEM_LETTERBOX, option.rect.y()+ITEM_LETTERBOX, scaled.width(), scaled.height()), QColor(10,10,10));
    painter->drawImage(option.rect.x()+ITEM_LETTERBOX, option.rect.y()+ITEM_LETTERBOX, scaled);

    painter->restore();
}

QSize PatternCollectionDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex & ) const {
    return QSize(ITEM_WIDTH, ITEM_HEIGHT);
}

