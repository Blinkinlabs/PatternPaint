#include "patternitem.h"
#include "undocommand.h"

#include <QPainter>
#include <QDebug>

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,255)

PatternItem::PatternItem(QSize patternSize, int frameCount, QListWidget* parent) :
    QListWidgetItem(parent, QListWidgetItem::UserType + 1),
    size(patternSize),
    modified(false)
{
    undoStack.setUndoLimit(50);

    // Note: The pattern is stored in a big image, with all frames placed next to each other.

    QImage newImage(size.width()*frameCount,
                    size.height(),
                    QImage::Format_ARGB32_Premultiplied);
    newImage.fill(COLOR_CANVAS_DEFAULT);
    applyUndoState(newImage);
}

void PatternItem::pushUndoState() {
    undoStack.push(new UndoCommand(image, this));
}

void PatternItem::applyUndoState(const QImage& newImage) {
    image = newImage;

    if(!notifier.isNull()) {
        notifier->signalSizeUpdated();
    }

    setModified(modified);
}

QString PatternItem::getPatternName() const
{
    if(fileInfo.baseName() == "") {
        return "Untitled";
    }
    return fileInfo.baseName();
}

bool PatternItem::load(const QFileInfo &newFileInfo)
{
    // TODO: Fail if there is unsaved data?

    // Attempt to load the iamge
    if(!replace(newFileInfo)) {
        return false;
    }

    // If successful, record the filename and clear the undo stack.
    fileInfo = newFileInfo;

    undoStack.clear();

    setModified(false);
    return true;
}

bool PatternItem::saveAs(const QFileInfo &newFileInfo) {
    // Attempt to save to this location
    if(!image.save(newFileInfo.absoluteFilePath())) {
        return false;
    }

    // If successful, update the filename
    fileInfo = newFileInfo;

    // TODO: Notify the main window that the filename was updated!
    //on_patternFilenameChanged(fileinfo);

    if(!notifier.isNull()) {
        notifier->signalNameUpdated();
    }

    setModified(false);
    return true;
}

bool PatternItem::replace(const QFileInfo &newFileInfo)
{
    pushUndoState();

    QImage newImage;

    // Attempt to load the iamge
    if(!newImage.load(newFileInfo.absoluteFilePath())) {
        return false;
    }

    // TODO: Warn if the source image wasn't of the expected aperture?
    image = newImage.scaledToHeight(size.height());
    if(image.width()%size.width() != 0) {
        // TODO: What?
    }

    if(!notifier.isNull()) {
        notifier->signalSizeUpdated();
    }

    setModified(true);
    return true;
}


bool PatternItem::save()
{
    if(!image.save(fileInfo.absoluteFilePath())) {
        return false;
    }

    // TODO: Set new save point here
    setModified(false);
    return true;
}

QVariant PatternItem::data(int role) const {
    switch(role) {
        case PreviewImage: return image;
        case Modified: return modified;
        case PatternSize: return image.size();
    };

    return QListWidgetItem::data(role);
}

void PatternItem::setData(int role, const QVariant& value) {
    switch(role)
    {
    case PreviewImage:
        applyUndoState(qvariant_cast<QImage>(value));
        break;
    case Modified:
        setModified(qvariant_cast<bool>(value));
        break;
    case PatternSize:
        Q_ASSERT(false);    // never set size separated from image!
        break;
    default:
        break;
    }

    QListWidgetItem::setData(role, value);
}

void PatternItem::resize(QSize newSize, bool scale) {
    if(size == newSize) {
        return;
    }

    pushUndoState();

    int frameCount = getFrameCount();
    size = newSize;

    QImage originalImage = image;

    // TODO: Proper scaling, need to crop/scale and copy each frame into the new image.
    originalImage = originalImage.scaled(size.width()*frameCount, size.height());

    // Initialize the pattern to a blank canvass
    image = QImage(size.width()*frameCount, size.height(),
                   QImage::Format_ARGB32_Premultiplied);
    image.fill(COLOR_CANVAS_DEFAULT);

    QPainter painter(&image);
    painter.drawImage(0,0,originalImage);

    if(!notifier.isNull()) {
        notifier->signalSizeUpdated();
    }

    setModified(true);
}

void PatternItem::applyInstrument(const QImage &update)
{
    pushUndoState();
    QPainter painter(&image);
    painter.drawImage(0,0,update);
    setModified(true);
}

void PatternItem::setModified(bool newModified)  {
    bool modifiedChanged = false;

    if(modified != newModified) {
        modifiedChanged = true;
    }

    modified = newModified;

    if(!notifier.isNull()) {
        notifier->signalDataUpdated();
        if(modifiedChanged) {
            notifier->signalModifiedChange();
        }
    }
}

void PatternItem::setNotifier(QPointer<PatternUpdateNotifier> newNotifier) {
    notifier = newNotifier;
}


const QImage& PatternItem::getFrame(int index) {
    // TODO: Protections here?

    frameData = QImage(size,QImage::Format_ARGB32_Premultiplied);

    // TODO: This is an inefficient way of achieving this.
    for(int x = 0; x < frameData.width(); x++) {
        for(int y = 0; y < frameData.height(); y++) {
            frameData.setPixel(x, y,
                              image.pixel(index*size.width() + x, y));
        }
    }

    return frameData;
}


int PatternItem::getFrameCount() const {
    return image.width()/size.width();
}


void PatternItem::deleteFrame(int index) {
    if(getFrameCount() < 2) {
        return;
    }

    if(index > getFrameCount() || index < 0) {
        return;
    }

    QImage newImage(image.width()-size.width(),
                    image.height(),
                    QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&newImage);

    if(index == 0) {
        // Crop the front from the original image
        painter.drawImage(0,0,image,1,0,-1,-1);
    }
    else if(index == image.width()-1) {
        // Crop the back from the original image
        painter.drawImage(0,0,image,0,0,-1,-1);
    }
    else {
        // Crop somewhere in the middle
        painter.drawImage(0,0,image,0,0,index*size.width(),-1);
        painter.drawImage(index*size.width(),0,image,(index+1)*size.width(),0,-1,-1);
    }

    image = newImage;
    applyInstrument(newImage);
}

void PatternItem::addFrame(int index) {
    if(index > getFrameCount() || index < 0) {
        return;
    }

    QImage newImage(image.width()+size.width(),
                    image.height(),
                    QImage::Format_ARGB32_Premultiplied);

    // copy data from the original image over
    // Initialize the pattern to a blank canvass
    newImage.fill(COLOR_CANVAS_DEFAULT);
    QPainter painter(&newImage);

    if(index == getFrameCount()-1) {
        // Add to the end of the image
        painter.drawImage(0,0,image,0,0,-1,-1);
    }
    else {
        // Crop somewhere in the middle
        painter.drawImage(0,0,image,0,0,(index+1)*size.width(),-1);
        painter.drawImage((index+2)*size.width(),0,image,(index+1)*size.width(),0,-1,-1);
    }

    image=newImage;
    applyInstrument(newImage);
}
