#include "patternscrollundocommand.h"

#include "patternscrollmodel.h"

PatternScrollUndoCommand::PatternScrollUndoCommand(PatternScrollModel *item, QUndoCommand *parent) :
    QUndoCommand(parent),
    patternScrollModel(item)
{
    previousImage = patternScrollModel->image;
    previousSize = patternScrollModel->frameSize;

    firstRun = true;
}

void PatternScrollUndoCommand::undo()
{
    currentImage = patternScrollModel->image;
    currentSize = patternScrollModel->frameSize;
    patternScrollModel->applyUndoState(previousImage, previousSize);
}

void PatternScrollUndoCommand::redo()
{
    // TODO: We're likely not handling undo/redo correctly if we need this?
    if (firstRun) {
        firstRun = false;
        return;
    }

    patternScrollModel->applyUndoState(currentImage, currentSize);
}
