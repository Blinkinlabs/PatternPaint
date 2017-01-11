#ifndef PATTERNCOLLECTIONDELEGATE_H
#define PATTERNCOLLECTIONDELEGATE_H

#include <QStyledItemDelegate>
#include "QPainter"
#include "QImage"

class PatternCollectionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    PatternCollectionDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
    {
        return NULL;
    }

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // PATTERNCOLLECTIONDELEGATE_H
