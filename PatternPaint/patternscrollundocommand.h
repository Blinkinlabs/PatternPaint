#ifndef PATTERNSCROLLUNDOCOMMAND_H
#define PATTERNSCROLLUNDOCOMMAND_H

#include <QUndoCommand>
#include <QImage>
#include <QPointer>

// TODO: Base this on actions, not blindly storing/restoring the state.

class PatternScrollModel;

/**
 * @brief Class which provides undo/redo actions
 *
 */
class PatternScrollUndoCommand : public QUndoCommand
{
public:
    PatternScrollUndoCommand(PatternScrollModel *item, QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    QImage previousImage;
    QImage currentImage;
    QSize previousSize;
    QSize currentSize;

    QPointer<PatternScrollModel> patternScrollModel;
    bool firstRun;
};

#endif // PATTERNSCROLLUNDOCOMMAND_H
