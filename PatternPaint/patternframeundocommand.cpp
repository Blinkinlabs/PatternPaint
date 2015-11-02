#include "patternframeundocommand.h"
#include "patternframemodel.h"


PatternFrameUndoCommand::PatternFrameUndoCommand(PatternFrameModel *item,
                         QUndoCommand *parent) :
    QUndoCommand(parent),
    patternFrameModel(item)
{
    previousFrames = patternFrameModel->frames;
    previousSize = patternFrameModel->frameSize;

    firstRun = true;
}

void PatternFrameUndoCommand::undo() {
    currentFrames = patternFrameModel->frames;
    currentSize = patternFrameModel->frameSize;
    patternFrameModel->applyUndoState(previousFrames, previousSize);
}

void PatternFrameUndoCommand::redo() {
    // TODO: We're likely not handling undo/redo correctly if we need this?
    if(firstRun) {
        firstRun = false;
        return;
    }

    patternFrameModel->applyUndoState(currentFrames, currentSize);
}
