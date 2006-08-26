#ifndef N_GUITEXTTABLE_H
#define N_GUITEXTTABLE_H
//------------------------------------------------------------------------------
/**
    @class nGuiTextTable
    @ingroup Gui

    @brief A table layout which implements a grid of texts.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiformlayout.h"
#include "util/narray2.h"
#include "gfx2/nfont2.h"

//------------------------------------------------------------------------------
class nGuiTextTable : public nGuiFormLayout
{
public:
    /// text alignments
    enum Alignment
    {
        Left,
        Center,
        Right,
    };
    /// constructor
    nGuiTextTable();
    /// destructor
    virtual ~nGuiTextTable();
    /// set font name
    void SetFontName(const char* fontName);
    /// get font name
    const char* GetFontName() const;
    /// set text border
    void SetBorder(float b);
    /// get text border
    float GetBorder() const;
    /// set the table dimension
    void SetDimensions(int numCols, int numRows);
    /// get table width
    int GetNumCols() const;
    /// get table height
    int GetNumRows() const;
    /// set column attributes
    void SetColAttrs(int colIndex, const vector4& textColor, Alignment align, float relWidth);
    /// set cell text
    void SetText(int colIndex, int rowIndex, const nString& text);
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// rendering
    virtual bool Render();

private:
    /// validate font
    void ValidateFont();
    /// get number of currently visible lines
    int GetNumVisibleLines();

    struct ColAttrs
    {
        vector4 textColor;
        Alignment align;
        float relWidth;
    };

    nRef<nFont2> refFont;
    float lineHeight;
    nString fontName;
    float border;

    nFixedArray<ColAttrs> columnAttributes;
    nArray2<nString> textArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextTable::SetBorder(float b)
{
    this->border = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiTextTable::GetBorder() const
{
    return this->border;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextTable::SetFontName(const char* fn)
{
    this->fontName = fn;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiTextTable::GetFontName() const
{
    return this->fontName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextTable::SetDimensions(int numCols, int numRows)
{
    this->columnAttributes.SetSize(numCols);
    this->textArray.SetSize(numCols, numRows);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiTextTable::GetNumRows() const
{
    return this->textArray.GetHeight();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiTextTable::GetNumCols() const
{
    return this->textArray.GetWidth();
}

//------------------------------------------------------------------------------
#endif

