#include "pattern.h"
#include "undocommand.h"
#include "patternframemodel.h"

#include <QDebug>
#include <QPainter>

Pattern::Pattern(QSize patternSize, int frameCount, QListWidget* parent) :
    QListWidgetItem(parent, QListWidgetItem::UserType + 1),
    frames(patternSize, parent),
    modified(false)
{
    frames.insertRows(0, frameCount);

    setModified(true);
}

QUndoStack *Pattern::getUndoStack()
{
    return frames.getUndoStack();
}

QString Pattern::getPatternName() const
{
    if(fileInfo.baseName() == "") {
        return "Untitled";
    }
    return fileInfo.baseName();
}

bool Pattern::load(const QFileInfo &newFileInfo)
{
    QImage sourceImage;

    // Attempt to load the iamge
    if(!sourceImage.load(newFileInfo.absoluteFilePath())) {
        return false;
    }

    // TODO: Warn if the source image wasn't of the expected aperture?
    sourceImage = sourceImage.scaledToHeight(frames.getSize().height());
    int newFrameCount = sourceImage.width()/frames.getSize().width();

    frames.removeRows(0,frames.getFrameCount());
    frames.insertRows(0,newFrameCount);

    QPainter painter;
    for(int i = 0; i < newFrameCount; i++) {
        QImage newFrameData(frames.getSize(), QImage::Format_ARGB32_Premultiplied);
        painter.begin(&newFrameData);
        painter.drawImage(QPoint(0,0), sourceImage,
                          QRect(frames.getSize().width()*i, 0, frames.getSize().width(),frames.getSize().height()));
        painter.end();
        frames.setData(frames.index(i,0),newFrameData,PatternFrameModel::FrameData);
    }

    // If successful, record the filename and clear the undo stack.
    fileInfo = newFileInfo;

    setModified(false);
    return true;
}

bool Pattern::saveAs(const QFileInfo &newFileInfo) {
    // Attempt to save to this location

    // Create a big image consisting of all the frames side-by-side
    QImage output(frames.getSize().width()*getFrameCount(), frames.getSize().height(),
                  QImage::Format_ARGB32_Premultiplied);

    // And copy the frames into it
    QPainter painter;
    painter.begin(&output);
    for(int i = 0; i < getFrameCount(); i++) {
        painter.drawImage(QPoint(frames.getSize().width()*i, 0),
                          getFrame(i));
    }
    painter.end();

    if(!output.save(newFileInfo.absoluteFilePath())) {
        return false;
    }

    // If successful, update the filename
    if(fileInfo != newFileInfo && !notifier.isNull()) {
        notifier->signalNameUpdated();
    }
    fileInfo = newFileInfo;

//    setModified(false);
    return true;
}

bool Pattern::save()
{
    return saveAs(fileInfo.absoluteFilePath());
}

bool Pattern::replace(const QFileInfo &newFileInfo)
{
//    pushUndoState();

//    QImage newImage;

//    // Attempt to load the iamge
//    if(!newImage.load(newFileInfo.absoluteFilePath())) {
//        return false;
//    }

//    // TODO: Warn if the source image wasn't of the expected aperture?
//    image = newImage.scaledToHeight(frames.getSize().height());
//    if(image.width()%frames.getSize().width() != 0) {
//        // TODO: What?
//    }

//    if(!notifier.isNull()) {
//        notifier->signalSizeUpdated();
//    }

//    setModified(true);
//    return true;
    return false;
}


QVariant Pattern::data(int role) const {
    switch(role) {
        case PreviewImage: return frames.data(frames.index(0),PatternFrameModel::FrameData);
    };

    return QListWidgetItem::data(role);
}

void Pattern::resize(QSize newSize, bool scale) {
    frames.resize(newSize, scale);
}

void Pattern::applyInstrument(int index, const QImage &update)
{
    frames.setData(frames.index(index),QVariant(update));
}

void Pattern::setModified(bool newModified)  {
    bool modifiedChanged = false;

    if(modified != newModified) {
        modifiedChanged = true;
    }

    modified = newModified;

    if(!notifier.isNull()) {
        if(modifiedChanged) {
            notifier->signalModifiedChange();
        }
    }
}

void Pattern::setNotifier(QPointer<PatternUpdateNotifier> newNotifier) {
    notifier = newNotifier;
}

const QImage Pattern::getFrame(int index) const {
    return frames.index(index).data(PatternFrameModel::FrameData).value<QImage>();
}

int Pattern::getFrameCount() const {
    return frames.rowCount();
}

void Pattern::deleteFrame(int index) {
    frames.removeRow(index);
}

void Pattern::addFrame(int index) {
    frames.insertRow(index);
}
