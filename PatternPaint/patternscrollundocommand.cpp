#include "patternscrollundocommand.h"

#include "patternscrollmodel.h"

PatternScrollUndoCommand::PatternScrollUndoCommand(PatternScrollModel *item, QUndoCommand *parent) :
    QUndoCommand(parent),
    patternScrollModel(item)
{
    previousState = patternScrollModel->state;

    firstRun = true;
}

void PatternScrollUndoCommand::undo()
{
    currentState = patternScrollModel->state;
    patternScrollModel->applyUndoState(previousState);
}

void PatternScrollUndoCommand::redo()
{
    if (firstRun) {
        firstRun = false;
        return;
    }

    patternScrollModel->applyUndoState(currentState);
}
