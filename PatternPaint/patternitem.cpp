#include "patternitem.h"

#include <QPainter>

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,0)

PatternItem::PatternItem(int patternLength, int ledCount, QListWidget* parent) :
    QListWidgetItem(parent, QListWidgetItem::UserType + 1),
    modified(false) {
    ustack.setUndoLimit(50);

    // TODO: Base me on passed parameters!
    QImage newImage(patternLength, ledCount, QImage::Format_ARGB32_Premultiplied);
    newImage.fill(COLOR_CANVAS_DEFAULT);
    setImage(newImage);
}

QVariant PatternItem::data(int role) const {
    switch(role) {
        case PreviewImage: return img;
        case Modified: return modified;
        case PatternSize: return psize;
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

void PatternItem::setImage(const QImage& image) {
    // TODO: Resize the image here.
    img = image;
    psize= img.size();
}

void PatternItem::resizeImage(int newPatternLength, int newLedCount, bool scale) {

    QImage originalImage = img;

    if(scale && newLedCount != originalImage.height()) {
        originalImage = originalImage.scaledToHeight(newLedCount);
    }

    // Initialize the pattern to a blank canvass
    img = QImage(newPatternLength,
                     newLedCount,
                     QImage::Format_ARGB32_Premultiplied);
    img.fill(COLOR_CANVAS_DEFAULT);

    QPainter painter(&img);
    painter.drawImage(0,0,originalImage);
}
