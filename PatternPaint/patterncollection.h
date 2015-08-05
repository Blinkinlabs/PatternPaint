#ifndef PATTERNCOLLECTION_H
#define PATTERNCOLLECTION_H

#include <QObject>
#include <QtWidgets>
#include <QListWidget>
#include <QPointer>
#include <QUndoGroup>
#include "patternupdatenotifier.h"

class PatternCollection : public QListWidget
{
    Q_OBJECT
public:
    PatternCollection(QWidget* parent);
    ~PatternCollection();

    // Handle re-ordering patterns by dragging, as well as file URLs dropped from other programs
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void addItem(QListWidgetItem* Item);

    // TODO: Reconsider ownership of these?
    void setUndoGroup(const QPointer<QUndoGroup> newUndoGroup) {undoGroup = newUndoGroup;}
    void setNotifier(const QPointer<PatternUpdateNotifier> newUpdateNotifier) {updateNotifier = newUpdateNotifier;}
private:
    QPointer<QUndoGroup> undoGroup;
    QPointer<PatternUpdateNotifier> updateNotifier;
};

#endif // PATTERNCOLLECTION_H
