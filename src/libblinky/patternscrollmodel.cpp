#include "patternscrollmodel.h"

#include "patternscrollundocommand.h"

#include <QColor>
#include <QDebug>
#include <QPainter>

PatternScrollModel::PatternScrollModel(QSize size, QObject *parent) :
    PatternModel(parent)
{
    state.frameSize = size;
    state.frameSpeed = PATTERN_FRAME_SPEED_DEFAULT_VALUE;
    state.modified = false;

    undoStack.setUndoLimit(50);

    // TODO: How to handle 0-sized image?
    state.image = QImage(0, state.frameSize.height(), QImage::Format_ARGB32_Premultiplied);
}

int PatternScrollModel::rowCount(const QModelIndex &) const
{
    return state.image.width();
}

Qt::ItemFlags PatternScrollModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return (Qt::ItemIsEnabled
            | Qt::ItemIsSelectable
            | Qt::ItemIsEditable);
}

Qt::DropActions PatternScrollModel::supportedDropActions() const
{
    return 0;
}

void PatternScrollModel::pushUndoState()
{
    // TODO: Implement me!
    undoStack.push(new PatternScrollUndoCommand(this));

    if (state.modified != true) {
        state.modified = true;

        QVector<int> roles;
        roles.append(Modified);
        emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
    }
}

void PatternScrollModel::applyUndoState(State newState)
{
    QVector<int> roles;

    if(state.image != newState.image)
        roles.append(FrameImage);
    if(state.frameSize != newState.frameSize)
        roles.append(FrameSize);
    if(state.frameSpeed != newState.frameSpeed)
        roles.append(FrameSpeed);
    if(state.fileName != newState.fileName)
        roles.append(FileName);
    if(state.modified != newState.modified)
        roles.append(Modified);

    state = newState;

    emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
}

QVariant PatternScrollModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount() || index.row() < 0)
        return QVariant();

    if (role == FrameImage || role == Qt::EditRole) {
        // TODO: Handle splits!
        QImage frame(state.frameSize, QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&frame);

        painter.setBrush(QBrush(QColor(0,0,0,255)));
        painter.drawRect(0,0,frame.width(),frame.height());

        if (index.row() < state.image.width() - state.frameSize.width()) {
            painter.drawImage(0, 0, state.image, index.row(), 0, state.frameSize.width(), state.frameSize.height());
        } else {
            painter.drawImage(0, 0,
                              state.image,
                              index.row(), 0,
                              state.image.width() - index.row(), state.frameSize.height());
            painter.drawImage(state.image.width() - index.row(), 0,
                              state.image,
                              0, 0,
                              state.frameSize.width() - state.image.width() + index.row(), state.frameSize.height());
        }

        painter.end();
        return frame;
    }

    if (role == EditImage)
        return state.image;

    if (role == FrameSize)
        return state.frameSize;

    if (role == FrameSpeed)
        return state.frameSpeed;

    if (role == FileName)
        return state.fileName;

    if (role == Modified)
        return state.modified;

    return QVariant();
}

bool PatternScrollModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    pushUndoState();

    if (role == EditImage) {
        QImage newImage = value.value<QImage>();

        if(newImage.size() != state.image.size())
            return false;

        QPainter painter;
        painter.begin(&state.image);
        painter.drawImage(0, 0, value.value<QImage>());
        painter.end();

        QVector<int> roles;
        roles.append(FrameImage);
        emit dataChanged(index, index, roles);
        return true;
    }

    if (role == FrameSize) {
        // TODO: Implement me

        state.frameSize = value.toSize();
        QImage newImage;
        bool scale = true;      // Enforce scaling...

        if (scale) {
            newImage = state.image.scaled(state.image.width(), state.frameSize.height());
        } else {
            newImage = QImage(state.image.width(), state.frameSize.height(),
                              QImage::Format_ARGB32_Premultiplied);
            newImage.fill(FRAME_COLOR_DEFAULT);

            QPainter painter(&newImage);
            painter.drawImage(0, 0, state.image);
        }

        state.image = newImage;

        QVector<int> roles;
        roles.append(FrameSize);
        roles.append(FrameImage);
        roles.append(EditImage);

        emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
        return true;
    }

    if (role == FrameSpeed) {
        state.frameSpeed = value.toFloat();

        QVector<int> roles;
        roles.append(FrameSpeed);
        emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
        return true;
    }

    if (role == FileName) {
        state.fileName = value.toString();

        QVector<int> roles;
        roles.append(FileName);
        emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
        return true;
    }

    if (role == Modified) {
        state.modified = value.toBool();

        QVector<int> roles;
        roles.append(Modified);
        emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
        return true;
    }

    return false;
}

bool PatternScrollModel::insertRows(int position, int rows, const QModelIndex &)
{
    if(position < 0)
        return false;

    if(position > state.image.width())
        return false;

    pushUndoState();
    beginInsertRows(QModelIndex(), position, position+rows-1);

    // Create a new image, then copy slice(s) of the original image into it.
    QImage newImage(state.image.width()+rows,
                    state.frameSize.height(),
                    QImage::Format_ARGB32_Premultiplied);

    newImage.fill(FRAME_COLOR_DEFAULT);

    QPainter painter;
    painter.begin(&newImage);

    if (position == 0) {
        painter.drawImage(rows, 0, state.image, 0, 0, state.image.width(), state.frameSize.height());
    } else if (position == state.image.width()) {
        painter.drawImage(0, 0, state.image, 0, 0, state.image.width(), state.frameSize.height());
    } else {
        painter.drawImage(0, 0, state.image, 0, 0, position, state.frameSize.height());
        painter.drawImage(position + rows, 0, state.image,
                          position, 0,
                          state.image.width() - position, state.frameSize.height());
    }

    painter.end();

    state.image = newImage;
    endInsertRows();

    // TODO: what does 'data changed' mean in these circumstances?
    QVector<int> roles;
    roles.append(FrameImage);
    emit dataChanged(this->index(position), this->index(position+rows), roles);
    return true;
}

bool PatternScrollModel::removeRows(int position, int rows, const QModelIndex &)
{
    if(position < 0)
        return false;

    if(position + rows > state.image.width())
        return false;

    pushUndoState();
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    QImage newImage(state.image.width()-rows, state.image.height(), QImage::Format_ARGB32_Premultiplied);
    newImage.fill(FRAME_COLOR_DEFAULT);

    // If we have any rows remaining, copy them into the new image
    if (newImage.width() > 0) {
        QPainter painter;
        painter.begin(&newImage);

        if (position == 0) {
            painter.drawImage(0, 0, state.image, rows, 0, state.image.width()-rows, state.frameSize.height());
        } else if (position == state.image.width()) {
            painter.drawImage(0, 0, state.image, 0, 0, state.image.width()-rows, state.frameSize.height());
        } else {
            painter.drawImage(0, 0, state.image, 0, 0, position, state.frameSize.height());
            painter.drawImage(position, 0, state.image,
                              position + rows, 0,
                              state.image.width() - position - rows, state.frameSize.height());
        }

        painter.end();
    }

    state.image = newImage;
    endRemoveRows();

    // TODO: what does 'data changed' mean in these circumstances?
    QVector<int> roles;
    roles.append(FrameImage);
    emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
    return true;
}

QDataStream &operator<<(QDataStream &stream, const PatternScrollModel &model)
{
    stream << model.state.frameSize;
    stream << model.state.fileName;
    stream << model.state.frameSpeed;
    stream << model.state.image;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, PatternScrollModel &model)
{
    PatternScrollModel::State newState;

    // TODO: Version first?
    stream >> newState.frameSize;
    stream >> newState.fileName;
    stream >> newState.frameSpeed;
    stream >> newState.image;

    // TODO: Data validation?

    // TODO: Not clear if the format actually makes a difference
    newState.image = newState.image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    model.state = newState;
    // TODO: Be noisy with messages, since our state just changed?

    return stream;
}
