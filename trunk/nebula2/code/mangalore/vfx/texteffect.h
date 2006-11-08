#ifndef VFX_TEXTEFFECT_H
#define VFX_TEXTEFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::TextEffect

    Renders a time-limited text "effect" into the world.

    (C) 2006 Radon Labs GmbH
*/
#include "vfx/effect.h"
#include "gfx2/nfont2.h"

//------------------------------------------------------------------------------
namespace VFX
{
class TextEffect : public Effect
{
    DeclareRtti;
    DeclareFactory(TextEffect);
public:
    /// alignments
    enum Alignment
    {
        Left,
        Center,
        Right,
    };
    /// constructor
    TextEffect();
    /// destructor
    virtual ~TextEffect();
    /// set displayed text
    void SetText(const nString& s);
    /// get displayed text
    const nString& GetText() const;
    /// set Nebula2 font name
    void SetFont(const nString& s);
    /// get Nebula2 font name
    const nString& GetFont() const;
    /// set text color
    void SetColor(const vector4& c);
    /// get text color
    const vector4& GetColor() const;
    /// start the effect
    virtual void OnStart();
    /// trigger the effect
    virtual void OnFrame();

protected:
    nRef<nFont2> refFont;
    nString text;
    nString fontName;
    vector4 color;
};

RegisterFactory(TextEffect);

//------------------------------------------------------------------------------
/**
*/
inline
void
TextEffect::SetText(const nString& s)
{
    this->text = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
TextEffect::GetText() const
{
    return this->text;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
TextEffect::SetFont(const nString& s)
{
    this->fontName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
TextEffect::GetFont() const
{
    return this->fontName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
TextEffect::SetColor(const vector4& c)
{
    this->color = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
TextEffect::GetColor() const
{
    return this->color;
}

} // namespace VFX
//------------------------------------------------------------------------------
#endif

