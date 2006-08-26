#ifndef N_EDITLINE_H
#define N_EDITLINE_H
//------------------------------------------------------------------------------
/**
    @class nEditLine
    @ingroup NebulaDataTypes

    @brief A line edit object.

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
    /// enable/disable filename mode
    void SetFileMode(bool b);
    /// get filename mode
    bool GetFileMode() const;
    /// toggle overstrike flag
    void ToggleOverstrike();
    /// set content
    void SetContent(const char* str);
    /// get content
    const char* GetContent() const;
    /// return true if empty
    bool IsEmpty() const;
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
    /// move cursor 1 word left
    void CursorWordLeft();
    /// move cursor 1 word right
    void CursorWordRight();
    /// delete character under cursor
    void Delete();
    /// delete character left of cursor
    void DeleteLeft();
    /// delete word left of cursor
    void DeleteWordLeft(const char* separators);
    /// return true if character is valid
    bool IsCharValid(uchar c) const;
    /// insert character at current cursor position
    void InsertChar(uchar c);
    /// insert a string
    void InsertString(const char* str);
    /// extract the word left and upto the cursor
    nString GetWordToCursor() const;

private:
    int bufSize;
    char* buffer;
    int cursorPos;
    bool overstrike;
    bool fileMode;
};

//------------------------------------------------------------------------------
/**
*/
inline
nEditLine::nEditLine(int maxChars) :
    bufSize(maxChars),
    buffer(0),
    cursorPos(0),
    overstrike(false),
    fileMode(false)
{
    this->buffer = n_new_array(char, maxChars);
    this->buffer[0] = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEditLine::~nEditLine()
{
    n_delete(this->buffer);
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
nEditLine::SetFileMode(bool b)
{
    this->fileMode = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEditLine::GetFileMode() const
{
    return this->fileMode;
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
nEditLine::ClearContent()
{
    this->buffer[0] = 0;
    this->cursorPos = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEditLine::IsEmpty() const
{
    return (0 == this->buffer[0]);
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
    this->cursorPos = 0;
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
    return (int) strlen(this->buffer);
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
    - 05-Mar-04     floh    bugfix if cursor at last position
*/
inline
void
nEditLine::Delete()
{
    if (this->cursorPos < this->GetMaxValidCursorPos())
    {
        char* moveTo = this->buffer + this->cursorPos;
        char* moveFrom = moveTo + 1;
        int moveCount = this->bufSize - (this->cursorPos + 1);
        memmove(moveTo, moveFrom, moveCount);
    }
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
nEditLine::DeleteWordLeft(const char* separators)
{
    n_assert(separators);
    while ((this->cursorPos > 0) && (strchr(separators, this->buffer[this->cursorPos - 1]) == 0))
    {
        --this->cursorPos;
        this->Delete();
    }
}

//------------------------------------------------------------------------------
/**
    10-Sep-04   floh    added point to list of illegal chars when in "fileMode"
*/
inline
bool
nEditLine::IsCharValid(uchar c) const
{
    if (this->fileMode)
    {
        // if in file name mode, filter out characters which are invalid for filenames
        if ((c == '\\') ||
            (c == '/') ||
            (c == ':') ||
            (c == '*') ||
            (c == '?') ||
            (c == '\"') ||
            (c == '<') ||
            (c == '>') ||
            (c == '|') ||
            (c == '.'))
        {
            return false;
        }
    }
    if (c >= 32)
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::InsertChar(uchar c)
{
    // only accept certain characters
    if (!this->IsCharValid(c))
    {
        return;
    }

    // Don't accept leading white space in file mode.
    if (this->fileMode)
    {
        if (c == ' ' && this->cursorPos == 0)
        {
            return;
        }
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
/**
*/
inline
void
nEditLine::InsertString(const char* str)
{
    n_assert(str);
    uchar c;
    while ((c = *str++))
    {
        this->InsertChar(c);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
nEditLine::GetWordToCursor() const
{
    nString str;
    int pos = this->cursorPos;
    while ((--pos >= 0) && this->buffer[pos] != ' ');
    pos++;
    int len = this->cursorPos - pos;
    if (len > 0)
    {
        str.Set(&(this->buffer[pos]), len);
    }
    return str;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEditLine::CursorWordLeft()
{
    while ((--this->cursorPos >= 0) && this->buffer[this->cursorPos] == ' ');
    while ((--this->cursorPos >= 0) && this->buffer[this->cursorPos] != ' ');
    this->cursorPos++;
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
nEditLine::CursorWordRight()
{
    int maxPos = this->GetMaxValidCursorPos();
    while ((++this->cursorPos < maxPos) && this->buffer[this->cursorPos] == ' ');
    while ((++this->cursorPos < maxPos) && this->buffer[this->cursorPos] != ' ');
    while ((++this->cursorPos < maxPos) && this->buffer[this->cursorPos] == ' ');
    if (this->cursorPos > maxPos)
    {
        this->cursorPos = maxPos;
    }
}

//------------------------------------------------------------------------------
#endif

