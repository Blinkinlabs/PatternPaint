#ifndef PATTERNSCROLLUNDOCOMMAND_H
#define PATTERNSCROLLUNDOCOMMAND_H

#include "libblinkyglobal.h"

#include "patternscrollmodel.h"

#include <QUndoCommand>
#include <QImage>
#include <QPointer>

// TODO: Base this on actions, not blindly storing/restoring the state.

/**
 * @brief Class which provides undo/redo actions
 *
 */
class LIBBLINKY_EXPORT PatternScrollUndoCommand : public QUndoCommand
{
public:
    PatternScrollUndoCommand(PatternScrollModel *item, QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    PatternScrollModel::State previousState;
    PatternScrollModel::State currentState;

    QPointer<PatternScrollModel> patternScrollModel;
    bool firstRun;
};

#endif // PATTERNSCROLLUNDOCOMMAND_H
