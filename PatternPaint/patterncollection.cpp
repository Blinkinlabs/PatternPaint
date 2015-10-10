#include "patterncollection.h"
#include "patternitem.h"
#include <QDebug>
#include <QSettings>

#define DEFAULT_DISPLAY_WIDTH 1
#define DEFAULT_DISPLAY_HEIGHT 60

PatternCollection::PatternCollection(QWidget *parent) :
    QListWidget(parent)
{

}

PatternCollection::~PatternCollection()
{

}

void PatternCollection::dragEnterEvent(QDragEnterEvent *event)
{
    // If this drag is coming from inside, then it's a re-order
    if(event->source() == this) {
        event->acceptProposedAction();
    }

    // If this drag has URLs, then try to load them as new patterns
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void PatternCollection::dropEvent(QDropEvent *event)
{
    if(event->source() == this) {
        // Note: Implement this by hand on Windows, to work around a bug in QT 5.4.1
#if defined Q_OS_WIN
        if(event->mimeData()->formats().at(0) == "application/x-qabstractitemmodeldatalist") {
            QByteArray itemData =
              event->mimeData()->data("application/x-qabstractitemmodeldatalist");
            QDataStream stream(&itemData, QIODevice::ReadOnly);

            int row, column;
            QMap<int, QVariant> v;
            stream >> row >> column >> v;

            // Insert the item in an appropriate place
            QModelIndex dropIndex = indexAt(event->pos());

            if(dropIndex.row() == row) {
                return;
            }

            QListWidgetItem* item = takeItem(row);
            if(dropIndex.row() == -1) {
                addItem(item);
            }
            else {
                int destinationRow = dropIndex.row();

                if(dropIndex.row() > row) {
                    destinationRow--;
                }
                insertItem(destinationRow, item);
            }
            setCurrentItem(item);
        }
#else
        QListWidget::dropEvent(event);
#endif
    }

    // If this drop has URLs, then try to load them as new patterns
    // TODO: Fix the data model and allow dropMimeData
    else if(event->mimeData()->hasUrls()) {
        QList<QUrl> droppedUrls = event->mimeData()->urls();
        int droppedUrlCnt = droppedUrls.size();
        for(int i = 0; i < droppedUrlCnt; i++) {
            QString localPath = droppedUrls[i].toLocalFile();
            QFileInfo fileInfo(localPath);

            // TODO: OS X Yosemite hack for /.file/id= references

            if(fileInfo.isFile()) {
                // Create a patternItem, and attempt to load the file
                // TODO: pass this to mainwindow for proper handling!
                QSettings settings;
                QSize displaySize;
                displaySize.setWidth(settings.value("Options/displayWidth", DEFAULT_DISPLAY_WIDTH).toUInt());
                displaySize.setHeight(settings.value("Options/displayHeight", DEFAULT_DISPLAY_HEIGHT).toUInt());

                PatternItem* patternItem = new PatternItem(displaySize, 1);

                if(!patternItem->load(fileInfo)) {
                    continue;
                }

                // Insert the item in an appropriate place
                QModelIndex dropIndex = indexAt(event->pos());
                if(dropIndex.row() == -1) {
                    addItem(patternItem);
                }
                else {
                    insertItem(dropIndex.row(), patternItem);
                }
                setCurrentItem(patternItem);
            }
        }
    }
}


void PatternCollection::addItem(QListWidgetItem* Item)
{
     PatternItem* patternItem = dynamic_cast<PatternItem*>(Item);

    // Add the item to the listwidget
    QListWidget::addItem(patternItem);

    // And register it with the UndoStack and notifier
    if(!undoGroup.isNull()) {
        undoGroup->addStack(patternItem->getUndoStack());
    }
    if(!updateNotifier.isNull()) {
        patternItem->setNotifier(updateNotifier);
    }
}
