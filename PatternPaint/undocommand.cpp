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

#include "undocommand.h"
#include "patterneditor.h"

UndoCommand::UndoCommand(const QImage& img, PatternEditor& editor, QUndoCommand *parent)
    : QUndoCommand(parent), mPrevImage(img), m_editor(editor)
{
    mCurrImage = mPrevImage;
}

void UndoCommand::undo()
{
    // TODO - add tools restore
    mCurrImage = m_editor.getPatternAsImage();
    m_editor.resetImage(mPrevImage);
    //mImageArea.saveImageChanges();
}

void UndoCommand::redo()
{
    m_editor.resetImage(mCurrImage);
}