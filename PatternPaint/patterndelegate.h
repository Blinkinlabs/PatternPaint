#ifndef PATTERNITEMDELEGATE_H
#define PATTERNITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "QPainter"
#include "QImage"


class PatternDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    PatternDelegate(QObject* parent = 0);
    virtual ~PatternDelegate();
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex & index ) const;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif // PATTERNITEMDELEGATE_H
