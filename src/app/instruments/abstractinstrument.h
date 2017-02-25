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

#include <QObject>
#include <QMouseEvent>
#include <QImage>
#include <QPointer>
#include <QCursor>
#include <QPixmap>
#include <QCursor>
#include <instrumentconfiguration.h>

#include "frameeditor.h"

/// Abstract instrument class.
class AbstractInstrument : public QObject
{
    Q_OBJECT

public:
    explicit AbstractInstrument(const QString &resource,
                                InstrumentConfiguration *instrumentConfiguration,
                                QObject *parent = 0);
    virtual ~AbstractInstrument()
    {
    }

    /// mousePressEvent
    /// @param event - mouse event
    /// @param FrameEditor - editor
    /// @param QPoint - logical position on image
    virtual void mousePressEvent(QMouseEvent *event, const QImage &frameData, const QPoint &) = 0;

    /// mouseMoveEvent
    /// @param event - mouse event
    /// @param FrameEditor - editor
    /// @param QPoint - logical position on image
    virtual void mouseMoveEvent(QMouseEvent *event, const QImage &frameData, const QPoint &pt) = 0;

    /// mouseReleaseEvent
    /// @param event - mouse event
    /// @param FrameEditor - editor
    /// @param QPoint - logical position on image
    virtual void mouseReleaseEvent(QMouseEvent *event, FrameEditor &, const QImage &frameData, const QPoint &pt) = 0;

    /// Get the mouse cursor for this instrument
    /// @return cursor for this tool
    virtual const QCursor & getCursor() const;

    /// Get the cursor preview for this instrument
    virtual const QImage &getPreview() const;

    /// True if the tool is currently in a drawing operation
    virtual bool isDrawing() const;

protected:
    QPointer<InstrumentConfiguration> instrumentConfiguration;

    QImage preview;     ///< Scratch space to draw tool output onto
    bool drawing;       ///< True if we have an unsaved

    /// Update the drawing preview. Default is a 1px cursor under the mouse
    virtual void updatePreview(const QImage &frameData, QPoint point);

    QPixmap icon;
    QCursor cursor;
};

#endif // ABSTRACTINSTRUMENT_H
