#include "pattern.h"
#include "patternframeundocommand.h"
#include "patternframemodel.h"
#include "patternscrollmodel.h"

#include <QDebug>
#include <QPainter>

Pattern::Pattern(PatternType type, QSize patternSize, int frameCount, QListWidget *parent) :
    QObject(parent),
    type(type)
{
    uuid = QUuid::createUuid();

    // TODO: Choose a pattern type!
    switch (type) {
    case FrameBased:
        frames = new PatternFrameModel(patternSize, this);
        playbackIndicator = false;
        timeline = true;
        break;
    case Scrolling:
        frames = new PatternScrollModel(patternSize, this);
        playbackIndicator = true;
        timeline = false;
        break;
    default:
        // ??
        break;
    }

    frames->insertRows(0, frameCount);
    frames->setData(frames->index(0), false, PatternModel::Modified);

    // TODO: MVC way of handling this?
    getUndoStack()->clear();
    frames->setData(frames->index(0), false, PatternModel::Modified);
}

QUndoStack *Pattern::getUndoStack()
{
    return frames->getUndoStack();
}

bool Pattern::hasValidFilename() const
{
    QString filename = frames->data(frames->index(0), PatternModel::FileName).toString();

    if (filename.isEmpty())
        return false;

    // If this is a Qt Resource, we can't save here
    if (filename.startsWith(":/"))
        return false;

    return true;
}

QString Pattern::getPatternName() const
{
    QFileInfo fileInfo(frames->data(frames->index(0), PatternModel::FileName).toString());

    if (fileInfo.baseName() == "")
        return "Untitled";
    return fileInfo.baseName();
}

bool Pattern::load(const QString &newFileName)
{
    QImage sourceImage;

    // Attempt to load the iamge
    if (!sourceImage.load(newFileName))
        return false;


    switch (type) {
    case FrameBased:
    {
        QSize frameSize = frames->data(frames->index(0), PatternModel::FrameSize).toSize();

        // TODO: Warn if the source image wasn't of the expected aperture?
        sourceImage = sourceImage.scaledToHeight(frameSize.height());
        int newFrameCount = sourceImage.width()/frameSize.width();

        frames->removeRows(0, frames->rowCount());
        frames->insertRows(0, newFrameCount);

        QPainter painter;
        for (int i = 0; i < newFrameCount; i++) {
            QImage newFrameData(frameSize, QImage::Format_ARGB32_Premultiplied);
            painter.begin(&newFrameData);
            painter.fillRect(newFrameData.rect(), QColor(0, 0, 0));
            painter.drawImage(QPoint(0, 0), sourceImage,
                              QRect(frameSize.width()*i, 0, frameSize.width(), frameSize.height()));
            painter.end();
            frames->setData(frames->index(i), newFrameData, PatternModel::FrameImage);
        }

        // If successful, record the filename and clear the undo stack.
        frames->setData(frames->index(0), newFileName, PatternModel::FileName);
        frames->setData(frames->index(0), false, PatternModel::Modified);

        break;
    }
    case Scrolling:
    {
        QSize frameSize = frames->data(frames->index(0), PatternModel::FrameSize).toSize();

        // TODO: Warn if the source image wasn't of the expected aperture?
        sourceImage = sourceImage.scaledToHeight(frameSize.height());
        int newFrameCount = sourceImage.width();

        frames->removeRows(0, frames->rowCount());

        frames->insertRows(0, newFrameCount);

        frames->setData(frames->index(0), sourceImage, PatternModel::EditImage);

        // If successful, record the filename and clear the undo stack.
        frames->setData(frames->index(0), newFileName, PatternModel::FileName);
        frames->setData(frames->index(0), false, PatternModel::Modified);

        break;
    }
    }

    // TODO: MVC way of handling this?
    getUndoStack()->clear();

    return true;
}

bool Pattern::saveAs(const QString newFileName)
{
    // Attempt to save to this location

    QSize frameSize = frames->data(frames->index(0), PatternModel::FrameSize).toSize();
    QImage output;

    switch (type) {
    case FrameBased:
    {
        // Create a big image consisting of all the frames side-by-side
        output = QImage(frameSize.width()*getFrameCount(), frameSize.height(),
                        QImage::Format_ARGB32_Premultiplied);

        // And copy the frames into it
        QPainter painter;
        painter.begin(&output);
        painter.fillRect(output.rect(), QColor(0, 0, 0));
        for (int i = 0; i < getFrameCount(); i++) {
            painter.drawImage(QPoint(frameSize.width()*i, 0),
                              getFrameImage(i));
        }
        painter.end();
        break;
    }
    case Scrolling:
    {
        // Just grab the edit image.
        output = frames->data(frames->index(0), PatternModel::EditImage).value<QImage>();
        break;
    }
    }

    if (!output.save(newFileName))
        return false;

    frames->setData(frames->index(0), newFileName, PatternModel::FileName);
    frames->setData(frames->index(0), false, PatternModel::Modified);

    return true;
}

bool Pattern::save()
{
    return saveAs(frames->data(frames->index(0), PatternModel::FileName).toString());
}

bool Pattern::getModified() const
{
    return frames->data(frames->index(0), PatternModel::Modified).toBool();
}

void Pattern::resize(QSize newSize, bool scale)
{
    Q_UNUSED(scale);

    frames->setData(frames->index(0), newSize, PatternModel::FrameSize);
}

QSize Pattern::getFrameSize() const
{
    return frames->data(frames->index(0), PatternModel::FrameSize).toSize();
}

const QImage Pattern::getFrameImage(int index) const
{
    return frames->index(index).data(PatternModel::FrameImage).value<QImage>();
}

void Pattern::setFrameImage(int index, const QImage &update)
{
    frames->setData(frames->index(index), QVariant(update), PatternModel::FrameImage);
}

const QImage Pattern::getEditImage(int index) const
{
    return frames->index(index).data(PatternModel::EditImage).value<QImage>();
}

void Pattern::setEditImage(int index, const QImage &update)
{
    frames->setData(frames->index(index), QVariant(update), PatternModel::EditImage);
}

int Pattern::getFrameCount() const
{
    return frames->rowCount();
}

float Pattern::getFrameSpeed() const
{
    return frames->index(0).data(PatternModel::FrameSpeed).toFloat();
}

void Pattern::setFrameSpeed(float speed)
{
    frames->setData(frames->index(0), speed, PatternModel::FrameSpeed);
}

void Pattern::deleteFrame(int index)
{
    if (getFrameCount() == 1)
        return;

    frames->removeRow(index);
}

void Pattern::addFrame(int index)
{
    frames->insertRow(index);
}
