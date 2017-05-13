#include "patternframemodel.h"

#include "patternframeundocommand.h"

#include <QColor>
#include <QDebug>
#include <QPainter>

PatternFrameModel::PatternFrameModel(QSize size, QObject *parent) :
    PatternModel(parent)
{
    state.frameSize = size;
    state.frameSpeed = PATTERN_FRAME_SPEED_DEFAULT_VALUE;
    state.modified = false;

    undoStack.setUndoLimit(50);
}

int PatternFrameModel::rowCount(const QModelIndex &) const
{
    return state.frames.count();
}

Qt::ItemFlags PatternFrameModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return (Qt::ItemIsEnabled
                | Qt::ItemIsDropEnabled);

    return (Qt::ItemIsEnabled
            | Qt::ItemIsSelectable
            | Qt::ItemIsEditable
            | Qt::ItemIsDragEnabled);
}

Qt::DropActions PatternFrameModel::supportedDropActions() const
{
    return (Qt::CopyAction
            | Qt::MoveAction);
}

void PatternFrameModel::pushUndoState()
{
    undoStack.push(new PatternFrameUndoCommand(this));

    if (state.modified == true)
        return;

    state.modified = true;

    QVector<int> roles;
    roles.append(Modified);
    emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
}

void PatternFrameModel::applyUndoState(State newState)
{
    QVector<int> roles;

    if(state.frames != newState.frames)
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

QVariant PatternFrameModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= state.frames.count())
        return QVariant();

    if (role == FrameImage || role == Qt::EditRole || role == EditImage)
        return state.frames.at(index.row());

    // TODO: Why are these stored here?
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

bool PatternFrameModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (index.row() >= state.frames.count())
        return false;

    // TODO: Only push an undo state if the operation succeeds?
    pushUndoState();

    if (role == Qt::EditRole || role == EditImage) {
        QImage newImage = value.value<QImage>();

        if(newImage.size() != state.frameSize)
            return false;

        // TODO: enforce size scaling here?
        state.frames.replace(index.row(), newImage);
        QVector<int> roles;
        roles.append(FrameImage);
        emit dataChanged(index, index, roles);
        return true;
    }

    if (role == FrameSize) {
        for (int row = 0; row < rowCount(); row++) {
            state.frameSize = value.toSize();
            QImage newImage = state.frames.at(row).scaled(state.frameSize);
            state.frames.replace(row, newImage);
        }

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
        // TODO: This should only be accessable through the undo stack?
        state.modified = value.toBool();

        QVector<int> roles;
        roles.append(Modified);
        emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
        return true;
    }

    return false;
}

bool PatternFrameModel::insertRows(int position, int rows, const QModelIndex &)
{
    if(position < 0)
        return false;

    if(position > state.frames.length())
        return false;

    pushUndoState();
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        QImage newImage(state.frameSize, QImage::Format_ARGB32_Premultiplied);
        newImage.fill(FRAME_COLOR_DEFAULT);
        state.frames.insert(position, newImage);
    }

    endInsertRows();

    // TODO: what does 'data changed' mean in these circumstances?
    QVector<int> roles;
    roles.append(FrameImage);
    emit dataChanged(this->index(position), this->index(position+rows), roles);
    return true;
}

bool PatternFrameModel::removeRows(int position, int rows, const QModelIndex &)
{
    if(position < 0)
        return false;

    if(position + rows > state.frames.length())
        return false;

    pushUndoState();
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row)
        state.frames.removeAt(position);

    endRemoveRows();

    // TODO: what does 'data changed' mean in these circumstances?
    QVector<int> roles;
    roles.append(FrameImage);
    emit dataChanged(this->index(0), this->index(rowCount()-1), roles);
    return true;
}

QDataStream &operator<<(QDataStream &stream, const PatternFrameModel &model)
{
    stream << model.state.frameSize;
    stream << model.state.fileName;
    stream << model.state.frameSpeed;
    stream << model.state.frames;

    return stream;
}


QDataStream &operator>>(QDataStream &stream, PatternFrameModel &model)
{
    PatternFrameModel::State newState;

    // TODO: Version first?
    stream >> newState.frameSize;
    stream >> newState.fileName;
    stream >> newState.frameSpeed;
    stream >> newState.frames;

    // TODO: Data validation?

    // TODO: Not clear if the format actually makes a difference
    for(QImage &frame : newState.frames)
        frame = frame.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    model.state = newState;
    // TODO: Be noisy with messages, since our state just changed?

    return stream;
}
