#ifndef N_GUITEXTLABEL_H
#define N_GUITEXTLABEL_H
//------------------------------------------------------------------------------
/**
    @class nGuiTextLabel
    @ingroup NebulaGuiSystem
    @brief Uses the text server to render a gui label which contains ASCII
    text.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguilabel.h"
#include "gfx2/nfont2.h"

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
    void SetFont(const char* fontName);
    /// get the font definition
    const char* GetFont() const;
    /// set text alignment
    void SetAlignment(Alignment a);
    /// get text alignment
    Alignment GetAlignment() const;
    /// enable/disable clipping (default is on)
    void SetClipping(bool b);
    /// get clipping flag
    bool GetClipping() const;
    /// set the text color
    void SetColor(const vector4& c);
    /// get the text color
    const vector4& GetColor() const;
    /// set the text shadow color
    void SetShadowColor(const vector4& c);
    /// get the text shadow color
    const vector4& GetShadowColor() const;
    /// set the shadow x/y offset
    void SetShadowOffset(const vector2& offset);
    /// get the shadow x/y offset
    const vector2& GetShadowOffset() const;
    /// set the text as integer
    void SetInt(int i);
    /// get the text as integer
    int GetInt() const;
    /// set border size in screen space units
    void SetBorder(const vector2& b);
    /// get border size in screen space units
    const vector2& GetBorder() const;

protected:
    /// render the label text
    void RenderText(bool pressed);

    nString fontName;
    nAutoRef<nGfxServer2> refGfxServer;
    nAutoRef<nResourceServer> refResourceServer;
    nRef<nFont2> refFont;       // the font resource
    nString text;               // the displayed text 
    vector4 color;
    vector4 shadowColor;
    vector2 shadowOffset;
    Alignment align;
    vector2 border;
    bool clipping;
};

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
nGuiTextLabel::SetShadowColor(const vector4& c)
{
    this->shadowColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiTextLabel::GetShadowColor() const
{
    return this->shadowColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextLabel::SetShadowOffset(const vector2& offset)
{
    this->shadowOffset = offset;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nGuiTextLabel::GetShadowOffset() const
{
    return this->shadowOffset;
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


