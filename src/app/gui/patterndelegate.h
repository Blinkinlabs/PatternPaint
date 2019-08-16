#ifndef PATTERNFRAMEDELEGATE_H
#define PATTERNFRAMEDELEGATE_H

#include <QStyledItemDelegate>
#include "QPainter"
#include "QImage"

class PatternDelegate : public QStyledItemDelegate
{
    Q_OBJECT

private:

    QSize getScaledWidgetSize(int height, QSize imageSize) const;
    QSize getScaledImageSize(int height, QSize imageSize) const;

public:
    PatternDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
    {
        return nullptr;
    }

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &modelIndex) const;
};

#endif // PATTERNFRAMEDELEGATE_H
