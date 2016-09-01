#include "patternframemodel.h"

#include "patternframeundocommand.h"

#include <QColor>
#include <QDebug>
#include <QPainter>

#define FRAME_COLOR_DEFAULT    QColor(0, 0, 0, 255)
#define PATTERN_FRAME_SPEED_DEFAULT_VALUE 10

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
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}

Qt::DropActions PatternFrameModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
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

bool PatternFrameModel::seriaize(QDataStream& stream)
{
    // TODO: push these into the state definition.
    stream << (quint32)FRAME_MODEL;
    stream << state.frameSize;
    stream << state.fileName;
    stream << state.frameSpeed;
    stream << state.frames;

    return true;
}

bool PatternFrameModel::deseriaize(QDataStream& stream)
{
    // TODO: push these into the state definition.
    stream >> state.frameSize;
    stream >> state.fileName;
    stream >> state.frameSpeed;
    stream >> state.frames;

    return true;
}


QVariant PatternFrameModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= state.frames.count() || index.row() < 0)
        return QVariant();

    if (role == FrameImage || role == Qt::EditRole || role == EditImage)
        return state.frames.at(index.row());

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

    pushUndoState();

    if (role == FrameImage || role == Qt::EditRole || role == EditImage) {
        // TODO: enforce size scaling here?
        state.frames.replace(index.row(), value.value<QImage>());
        QVector<int> roles;
        roles.append(FrameImage);
        emit dataChanged(index, index, roles);
        return true;
    }

    if (role == FrameSize) {
        for (int row = 0; row < rowCount(); row++) {
            state.frameSize = value.toSize();
            QImage newImage;
            bool scale = true;      // Enforce scaling...

            if (scale) {
                newImage = state.frames.at(row).scaled(state.frameSize);
            } else {
                newImage = QImage(state.frameSize,
                                  QImage::Format_ARGB32_Premultiplied);
                newImage.fill(FRAME_COLOR_DEFAULT);

                QPainter painter(&newImage);
                painter.drawImage(0, 0, state.frames.at(row));
            }

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
