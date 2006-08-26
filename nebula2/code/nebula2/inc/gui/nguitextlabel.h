#ifndef N_GUITEXTLABEL_H
#define N_GUITEXTLABEL_H
//------------------------------------------------------------------------------
/**
    @class nGuiTextLabel
    @ingroup Gui
    @brief Uses the text server to render a gui label which contains ASCII
    text.

    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguilabel.h"
#include "gfx2/nfont2.h"
#include "resource/nresourceserver.h"

//------------------------------------------------------------------------------
class nGuiTextLabel : public nGuiLabel
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
    nGuiTextLabel();
    /// destructor
    virtual ~nGuiTextLabel();
    /// render the widget
    virtual bool Render();
    /// set the text as string
    virtual void SetText(const char* text);
    /// get the text as string
    virtual const char* GetText() const;
    /// set the font to use (see nGuiServer::AddFont())
    virtual void SetFont(const char* fontName);
    /// get the font definition
    const char* GetFont() const;
    /// set text alignment
    void SetAlignment(Alignment a);
    /// get text alignment
    Alignment GetAlignment() const;
    /// set vertical centering
    void SetVCenter(bool b);
    /// get vertical centering
    bool GetVCenter() const;
    /// enable/disable word break
    void SetWordBreak(bool b);
    /// get word break
    bool GetWordBreak() const;
    /// enable/disable clipping (default is on)
    void SetClipping(bool b);
    /// get clipping flag
    bool GetClipping() const;
    /// set the text color
    virtual void SetColor(const vector4& c);
    /// get the text color
    const vector4& GetColor() const;
    /// set pressed text offset
    void SetPressedOffset(const vector2& v);
    /// get pressed text offset
    const vector2& GetPressedOffset() const;
    /// set the text as integer
    void SetInt(int i);
    /// get the text as integer
    int GetInt() const;
    /// set border size in screen space units
    void SetBorder(const vector2& b);
    /// get border size in screen space units
    const vector2& GetBorder() const;
    /// computes the text extent for this widget
    vector2 GetTextExtent();
    /// Set the textcolor for blinking state
    void SetBlinkingColor(vector4 color);
    /// Get the textcolor for blinking state
    vector4 GetBlinkingColor() const;

protected:
    /// render the label text
    virtual void RenderText(bool pressed);
    /// (re-)validate the font object
    void ValidateFont();

    nString fontName;
    nRef<nFont2> refFont;       // the font resource
    nString text;               // the displayed text
    vector4 color;
    vector4 blinkColor;
    vector2 pressedOffset;
    Alignment align;
    vector2 border;
    bool clipping;
    bool wordBreak;
    bool vCenter;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetBlinkingColor(vector4 color)
{
    this->blinkColor = color;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nGuiTextLabel::GetBlinkingColor() const
{
    return this->blinkColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetVCenter(bool b)
{
    this->vCenter = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiTextLabel::GetVCenter() const
{
    return this->vCenter;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetWordBreak(bool b)
{
    this->wordBreak = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiTextLabel::GetWordBreak() const
{
    return this->wordBreak;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetPressedOffset(const vector2& v)
{
    this->pressedOffset = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nGuiTextLabel::GetPressedOffset() const
{
    return this->pressedOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetClipping(bool b)
{
    this->clipping = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiTextLabel::GetClipping() const
{
    return this->clipping;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetFont(const char* name)
{
    n_assert(name);
    this->fontName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiTextLabel::GetFont() const
{
    return this->fontName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetColor(const vector4& c)
{
    this->color = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiTextLabel::GetColor() const
{
    return this->color;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetInt(int i)
{
    char buf[128];
    sprintf(buf, "%d", i);
    this->SetText(buf);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiTextLabel::GetInt() const
{
    return atoi(this->GetText());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetAlignment(Alignment a)
{
    this->align = a;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiTextLabel::Alignment
nGuiTextLabel::GetAlignment() const
{
    return this->align;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetBorder(const vector2& b)
{
    this->border = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nGuiTextLabel::GetBorder() const
{
    return this->border;
}

//------------------------------------------------------------------------------
#endif


