#ifndef N_EDITLINE_H
#define N_EDITLINE_H
//------------------------------------------------------------------------------
/**
    @class nEditLine

    A line edit object.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
class nEditLine
{
public:
    /// constructor
    nEditLine(int maxLen);
    /// destructor
    ~nEditLine();
    /// get max line length
    int GetMaxLineLength() const;
    /// turn overstrike on/off
    void SetOverstrike(bool b);
    /// get overstrike flag
    bool GetOverstrike() const;
    /// toggle overstrike flag
    void ToggleOverstrike();
    /// set content
    void SetContent(const char* str);
    /// get content
    const char* GetContent() const;
    /// clear content
    void ClearContent();
    /// get max valid cursor position
    int GetMaxValidCursorPos() const;
    /// set cursor position
    void SetCursorPos(int pos);
    /// get cursor position
    int GetCursorPos() const;
    /// set cursor position to begin of line
    void CursorHome();
    /// set cursor position to end of line
    void CursorEnd();
    /// move cursor to the left
    void CursorLeft();
    /// move cursor to the right
    void CursorRight();
    /// delete character under cursor
    void Delete();
    /// delete character left of cursor
    void DeleteLeft();
    /// insert character at current cursor position
    void InsertChar(uchar c);

private:
    int bufSize;
    char* buffer;
    int cursorPos;
    bool overstrike;
};

//------------------------------------------------------------------------------
/**
*/
inline
nEditLine::nEditLine(int maxChars) :
    bufSize(maxChars),
    buffer(0),
    cursorPos(0),
    overstrike(false)
{
    this->buffer = new char[maxChars];
    this->buffer[0] = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEditLine::~nEditLine()
{
    delete this->buffer;
    this->buffer = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nEditLine::GetMaxLineLength() const
{
    return this->bufSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::SetOverstrike(bool b)
{
    this->overstrike = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEditLine::GetOverstrike() const
{
    return this->overstrike;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::ToggleOverstrike()
{
    this->overstrike = !this->overstrike;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::SetContent(const char* str)
{
    n_assert(str);
    n_strncpy2(this->buffer, str, this->bufSize);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nEditLine::GetContent() const
{
    return this->buffer;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nEditLine::GetMaxValidCursorPos() const
{
    return strlen(this->buffer);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::SetCursorPos(int pos)
{
    n_assert((pos >= 0) && (pos <= this->GetMaxValidCursorPos()));
    this->cursorPos = pos;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nEditLine::GetCursorPos() const
{
    return this->cursorPos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::CursorHome()
{
    this->cursorPos = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::CursorEnd()
{
    this->cursorPos = this->GetMaxValidCursorPos();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::CursorLeft()
{
    this->cursorPos--;
    if (this->cursorPos < 0)
    {
        this->cursorPos = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::CursorRight()
{
    this->cursorPos++;
    if (this->cursorPos > this->GetMaxValidCursorPos())
    {
        this->cursorPos = this->GetMaxValidCursorPos();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::Delete()
{
    char* moveTo = this->buffer + this->cursorPos;
    char* moveFrom = moveTo + 1;
    int moveCount = this->bufSize - (this->cursorPos + 1);
    memmove(moveTo, moveFrom, moveCount);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::DeleteLeft()
{
    if (this->cursorPos > 0)
    {
        --this->cursorPos;
        this->Delete();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::InsertChar(uchar c)
{
    // ignore everything below space
    if (c < 32)
    {
        return;
    }

    // overstrike or insert mode?
    if (this->overstrike)
    {
        // make sure string remains terminated
        if ((0 == this->buffer[this->cursorPos]) && (this->cursorPos < (this->bufSize - 1)))
        {
            this->buffer[this->cursorPos + 1] = 0;
        }
    }
    else
    {
        // insert mode: move string right of cursor
        char* moveFrom = this->buffer + this->cursorPos;
        char* moveTo   = moveFrom + 1;
        int moveCount  = this->bufSize - (this->cursorPos + 1); // preserve trailing 0
        memmove(moveTo, moveFrom, moveCount);
    }
    ((uchar*)this->buffer)[this->cursorPos++] = c;
    if (this->cursorPos >= (this->bufSize - 1))
    {
        this->cursorPos = this->bufSize - 1;
    }
    
    // make sure the buffer is always properly terminated
    this->buffer[this->bufSize - 1] = 0;
}

//------------------------------------------------------------------------------
#endif
    