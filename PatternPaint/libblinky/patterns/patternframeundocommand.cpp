#include "patternframeundocommand.h"

#include "patternframemodel.h"

PatternFrameUndoCommand::PatternFrameUndoCommand(PatternFrameModel *item, QUndoCommand *parent) :
    QUndoCommand(parent),
    patternFrameModel(item)
{
    previousState = patternFrameModel->state;

    firstRun = true;
}

void PatternFrameUndoCommand::undo()
{
    currentState = patternFrameModel->state;
    patternFrameModel->applyUndoState(previousState);
}

void PatternFrameUndoCommand::redo()
{
    if (firstRun) {
        firstRun = false;
        return;
    }

    patternFrameModel->applyUndoState(currentState);
}
