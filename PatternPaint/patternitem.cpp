#include "patternitem.h"

PatternItem::PatternItem(QListWidget* parent) :
    QListWidgetItem(parent, QListWidgetItem::UserType + 1),
    modified(false) {
        ustack.setUndoLimit(50);
}

PatternItem::PatternItem(const QString& text) :
    QListWidgetItem(text, 0, QListWidgetItem::UserType + 1),
    modified(false) {

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
        img = qvariant_cast<QImage>(value);
        psize = img.size();
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
    img = image;
    psize= img.size();
}

bool PatternItem::saveToFile(const QString& fileName) {
    if (img.save(fileName)) {
        filename = fileName;
        setToolTip(filename);
        return true;
    }

    return false;
}
