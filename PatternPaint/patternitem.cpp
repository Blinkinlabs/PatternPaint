#include "patternitem.h"
#include "undocommand.h"

#include <QPainter>
#include <QDebug>

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,0)

PatternItem::PatternItem(int patternLength, int ledCount, QListWidget* parent) :
    QListWidgetItem(parent, QListWidgetItem::UserType + 1),
    modified(false) {
    undoStack.setUndoLimit(50);

    QImage newImage(patternLength, ledCount, QImage::Format_ARGB32_Premultiplied);
    newImage.fill(COLOR_CANVAS_DEFAULT);
    setImage(newImage);
}

PatternItem::PatternItem(int patternLength, int ledCount, QImage newImage, QListWidget* parent) :
    QListWidgetItem(parent, QListWidgetItem::UserType + 1),
    modified(false) {
    undoStack.setUndoLimit(50);

    image = newImage;
    resize(patternLength, ledCount, true);
}

void PatternItem::pushUndoState() {
    qDebug() << "Pushing undo state";
    undoStack.push(new UndoCommand(image, this));
    modified = true;
}

void PatternItem::popUndoState() {
    qDebug() << "Popping undo state";
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
        setImage(qvariant_cast<QImage>(value));
        break;
    case Modified:
        modified = qvariant_cast<bool>(value);
        break;
    case PatternSize:
        Q_ASSERT(false);    // never set size separated from image!
        break;
    default:
        break;
    }

    QListWidgetItem::setData(role, value);
}

void PatternItem::setImage(const QImage& newImage) {
    image = newImage;
}

void PatternItem::resize(int newPatternLength, int newLedCount, bool scale) {
    pushUndoState();

    QImage originalImage = image;

    if(scale && newLedCount != originalImage.height()) {
        originalImage = originalImage.scaledToHeight(newLedCount);
    }

    // Initialize the pattern to a blank canvass
    image = QImage(newPatternLength,
                     newLedCount,
                     QImage::Format_ARGB32_Premultiplied);
    image.fill(COLOR_CANVAS_DEFAULT);

    QPainter painter(&image);
    painter.drawImage(0,0,originalImage);
}

void PatternItem::flipHorizontal()
{
    pushUndoState();
    image = image.mirrored(true, false);
}

void PatternItem::flipVertical()
{
    pushUndoState();
    image = image.mirrored(false, true);
}

void PatternItem::applyInstrument(QImage &update)
{
    pushUndoState();
    QPainter painter(&image);
    painter.drawImage(0,0,update);
}

void PatternItem::clear()
{
    pushUndoState();
    image.fill(COLOR_CANVAS_DEFAULT);
}

void PatternItem::setModified(bool newModified)  {
    modified = newModified;
//        emit changed(m_edited);
}
