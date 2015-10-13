#ifndef PATTERNLISTWIDGET_H
#define PATTERNLISTWIDGET_H

#include <QObject>
#include <QtWidgets>
#include <QListWidget>
#include <QPointer>
#include <QUndoGroup>
#include "patternupdatenotifier.h"
#include "pattern.h"

class PatternListWidget : public QListWidget
{
    Q_OBJECT
public:
    PatternListWidget(QWidget* parent);
    ~PatternListWidget();

    // Handle re-ordering patterns by dragging, as well as file URLs dropped from other programs
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void addItem(QListWidgetItem* Item);

    // TODO: Reconsider ownership of these?
    void setUndoGroup(const QPointer<QUndoGroup> newUndoGroup) {undoGroup = newUndoGroup;}
    void setNotifier(const QPointer<PatternUpdateNotifier> newUpdateNotifier) {updateNotifier = newUpdateNotifier;}

    /// True if there is a valid pattern selected
    bool hasPattern() const;

    /// Get the current pattern
    Pattern *pattern() const;

private:
    QPointer<QUndoGroup> undoGroup;
    QPointer<PatternUpdateNotifier> updateNotifier;
};

#endif // PATTERNLISTWIDGET_H
