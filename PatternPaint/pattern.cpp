#include "pattern.h"
#include "undocommand.h"

Pattern::Pattern(QSize patternSize, int frameCount, QListWidget* parent) :
    QListWidgetItem(parent, QListWidgetItem::UserType + 1),
    frames(patternSize, parent),
    modified(false)
{
    undoStack.setUndoLimit(50);

    frames.insertRows(0, frameCount);
}

void Pattern::pushUndoState() {
    //undoStack.push(new UndoCommand(image, this));
}

void Pattern::applyUndoState(const QImage& newImage) {

    if(!notifier.isNull()) {
        notifier->signalSizeUpdated();
    }

    setModified(modified);
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

bool Pattern::saveAs(const QFileInfo &newFileInfo) {
    // Attempt to save to this location
//    if(!image.save(newFileInfo.absoluteFilePath())) {
//        return false;
//    }

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


bool Pattern::save()
{
//    if(!image.save(fileInfo.absoluteFilePath())) {
//        return false;
//    }

//    // TODO: Set new save point here
//    setModified(false);
//    return true;
    return false;
}

QVariant Pattern::data(int role) const {
    switch(role) {
        case PreviewImage: return frames.data(frames.index(0),Qt::DisplayRole);
        case Modified: return modified;
        case PatternSize: return frames.getSize();
    };

    return QListWidgetItem::data(role);
}

void Pattern::setData(int role, const QVariant& value) {
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
        notifier->signalDataUpdated();
        if(modifiedChanged) {
            notifier->signalModifiedChange();
        }
    }
}

void Pattern::setNotifier(QPointer<PatternUpdateNotifier> newNotifier) {
    notifier = newNotifier;
}

const QImage Pattern::getFrame(int index) {
    QModelIndex modelIndex = frames.index(index);
    return modelIndex.data(Qt::DisplayRole).value<QImage>();
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
