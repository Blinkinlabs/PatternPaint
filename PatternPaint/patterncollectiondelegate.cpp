#include "patterncollectiondelegate.h"

#include "patterncollectionmodel.h"
#include "defaults.h"

#include <QDebug>

#define ITEM_HEIGHT 120
#define ITEM_WIDTH 150
#define ITEM_LETTERBOX 10
#define FRAME_SIZE 2

PatternCollectionDelegate::PatternCollectionDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void PatternCollectionDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    painter->save();
    QImage img = qvariant_cast<QImage>(index.data(PatternCollectionModel::PreviewImage));

    QSize previewSize(ITEM_WIDTH - 2*ITEM_LETTERBOX, ITEM_HEIGHT - 2*ITEM_LETTERBOX);
    QImage scaled = img.scaled(previewSize, Qt::KeepAspectRatioByExpanding);

// QStyledItemDelegate::paint(painter, option, index);

    // If this pattern is active, draw it as selected

    QColor frameColor = COLOR_FRAME_UNSELECTED;

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, COLOR_BACKGROUND_CHECKED);
        frameColor = COLOR_FRAME_SELECTED;
    }

    // Draw the frame
    painter->fillRect(QRect(option.rect.x()+ITEM_LETTERBOX-FRAME_SIZE,
                            option.rect.y()+ITEM_LETTERBOX-FRAME_SIZE,
                            previewSize.width()+2*FRAME_SIZE,
                            previewSize.height()+2*FRAME_SIZE), frameColor);

    // Draw the image
    painter->drawImage(option.rect.x()+ITEM_LETTERBOX, option.rect.y()+ITEM_LETTERBOX, scaled,
                       (scaled.width() - previewSize.width())/2, (scaled.height() - previewSize.height())/2,
                       previewSize.width(), previewSize.height());

    painter->restore();
}

QSize PatternCollectionDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return QSize(ITEM_WIDTH, ITEM_HEIGHT);
}
