#ifndef PATTERNFRAMESLISTVIEW_H
#define PATTERNFRAMESLISTVIEW_H

#include <QListView>

class PatternFrameListView : public QListView
{
public:
    PatternFrameListView(QWidget *parent);
    ~PatternFrameListView();

    void resizeEvent(QResizeEvent *event);
};

#endif // PATTERNFRAMESLISTVIEW_H
