#include "patternframelistview.h"

#include <QDebug>

PatternFrameListView::PatternFrameListView(QWidget *parent) :
    QListView(parent)
{
}

PatternFrameListView::~PatternFrameListView()
{
}

void PatternFrameListView::resizeEvent(QResizeEvent *event)
{
    // Propigate the new size to the delegate
    emit this->itemDelegate()->sizeHintChanged(QModelIndex());

    QListView::resizeEvent(event);
}
