#ifndef LETTERBOXSCROLLAREA_H
#define LETTERBOXSCROLLAREA_H

#include <QScrollArea>


/// Provides a scroll area that uses an aspect ratio hint from the child widget
/// to enforce an aspect ratio based on the new window height
///
/// The aspect ratio is determined from the 'baseHeight' and 'baseWidth' settings
/// in the child widget, as a hack.
class LetterboxScrollArea : public QScrollArea
{
public:
    LetterboxScrollArea(QWidget *parent);

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // LETTERBOXSCROLLAREA_H
