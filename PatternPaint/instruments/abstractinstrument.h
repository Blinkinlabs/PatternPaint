/*
 * This source file is part of EasyPaint.
 *
 * Copyright (c) 2012 EasyPaint <https://github.com/Gr1N/EasyPaint>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ABSTRACTINSTRUMENT_H
#define ABSTRACTINSTRUMENT_H

#include <QtCore/QObject>
#include <QMouseEvent>
#include <QImage>
#include <QCursor>
#include <QPixmap>
#include <QCursor>

class FrameEditor;

/// Abstract instrument class.
class AbstractInstrument : public QObject
{
    Q_OBJECT

public:
    explicit AbstractInstrument(QObject *parent = 0);
    virtual ~AbstractInstrument()
    {
    }

    /// mousePressEvent
    /// @param event - mouse event
    /// @param FrameEditor - editor
    /// @param QPoint - logical position on image
    virtual void mousePressEvent(QMouseEvent *event, FrameEditor &, const QPoint &) = 0;

    /// mousePressEvent
    /// @param event - mouse event
    /// @param FrameEditor - editor
    /// @param QPoint - logical position on image
    virtual void mouseMoveEvent(QMouseEvent *event, FrameEditor &, const QPoint &pt) = 0;

    /// mousePressEvent
    /// @param event - mouse event
    /// @param FrameEditor - editor
    /// @param QPoint - logical position on image
    virtual void mouseReleaseEvent(QMouseEvent *event, FrameEditor &, const QPoint &pt) = 0;

    /// Get the mouse cursor for this instrument
    /// @return cursor for this tool
    virtual QCursor cursor() const = 0;

    /// Check if the tool has preview data to display
    /// @return true if the instrument has preview data to display
    virtual bool showPreview() const
    {
        return drawing;
    }

    virtual const QImage &getPreview() const
    {
        return toolPreview;
    }

signals:

protected:
    QPoint mStartPoint, mEndPoint; ///< Point for events.
    QImage toolPreview; ///< Scratch space to draw tool output onto
    bool drawing;       ///< True if we have an unsaved

    virtual void paint(FrameEditor &) = 0;
};

/// Class for managing the mouse cursor
class CustomCursorInstrument : public AbstractInstrument
{
    Q_OBJECT
public:
    CustomCursorInstrument(const QString &resource, QObject *parent = 0);
    virtual QCursor cursor() const;
private:
    QPixmap mpm;
    QCursor mcur;
};

#endif // ABSTRACTINSTRUMENT_H
