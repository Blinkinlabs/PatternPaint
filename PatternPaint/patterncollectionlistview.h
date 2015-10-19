#ifndef PATTERNCOLLECTIONLISTVIEW_H
#define PATTERNCOLLECTIONLISTVIEW_H

#include <QObject>
#include <QtWidgets>
#include <QListWidget>
#include <QPointer>
#include <QUndoGroup>
#include "patternupdatenotifier.h"
#include "pattern.h"

class PatternCollectionListView : public QListView
{
    Q_OBJECT
public:
    PatternCollectionListView(QWidget* parent);
    ~PatternCollectionListView();

//    // Handle re-ordering patterns by dragging, as well as file URLs dropped from other programs
//    void dragEnterEvent(QDragEnterEvent *event);
//    void dropEvent(QDropEvent *event);

private:

};

#endif // PATTERNCOLLECTIONLISTVIEW_H
