#include "pattern.h"
#include "undocommand.h"
#include "patternframemodel.h"

#include <QDebug>
#include <QPainter>

Pattern::Pattern(QSize patternSize, int frameCount, QListWidget* parent) :
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

    QSize frameSize = frames.data(frames.index(0),PatternFrameModel::FrameSize).toSize();

    // TODO: Warn if the source image wasn't of the expected aperture?
    sourceImage = sourceImage.scaledToHeight(frameSize.height());
    int newFrameCount = sourceImage.width()/frameSize.width();

    frames.removeRows(0,frames.rowCount());
    frames.insertRows(0,newFrameCount);

    QPainter painter;
    for(int i = 0; i < newFrameCount; i++) {
        QImage newFrameData(frameSize, QImage::Format_ARGB32_Premultiplied);
        painter.begin(&newFrameData);
        painter.drawImage(QPoint(0,0), sourceImage,
                          QRect(frameSize.width()*i, 0, frameSize.width(),frameSize.height()));
        painter.end();
        frames.setData(frames.index(i),newFrameData,PatternFrameModel::FrameData);
    }

    // If successful, record the filename and clear the undo stack.
    fileInfo = newFileInfo;

    setModified(false);
    return true;
}

bool Pattern::saveAs(const QFileInfo &newFileInfo) {
    // Attempt to save to this location

    QSize frameSize = frames.data(frames.index(0),PatternFrameModel::FrameSize).toSize();

    // Create a big image consisting of all the frames side-by-side
    QImage output(frameSize.width()*getFrameCount(), frameSize.height(),
                  QImage::Format_ARGB32_Premultiplied);

    // And copy the frames into it
    QPainter painter;
    painter.begin(&output);
    for(int i = 0; i < getFrameCount(); i++) {
        painter.drawImage(QPoint(frameSize.width()*i, 0),
                          getFrame(i));
    }
    painter.end();

    if(!output.save(newFileInfo.absoluteFilePath())) {
        return false;
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


void Pattern::resize(QSize newSize, bool scale) {
    frames.setData(frames.index(0),newSize, PatternFrameModel::FrameSize);
}

void Pattern::replaceFrame(int index, const QImage &update)
{
    frames.setData(frames.index(index),QVariant(update), PatternFrameModel::FrameData);
}

void Pattern::setModified(bool newModified)  {
    bool modifiedChanged = false;

    if(modified != newModified) {
        modifiedChanged = true;
    }

    modified = newModified;
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
