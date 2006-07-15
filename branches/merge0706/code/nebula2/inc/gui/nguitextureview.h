#ifndef N_GUITEXTUREVIEW_H
#define N_GUITEXTUREVIEW_H
//------------------------------------------------------------------------------
/**
    @class nGuiTextureView
    @ingroup Gui
    @brief A label widget which directly renders a texture. Used by the texture
    browser.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

class nTexture2;

//------------------------------------------------------------------------------
class nGuiTextureView : public nGuiWidget
{
public:
    /// constructor
    nGuiTextureView();
    /// destructor
    virtual ~nGuiTextureView();
    /// set texture to display
    void SetTexture(nTexture2* tex);
    /// set color to multiply with the texture
    void SetColor(vector4& colorToSet);
    /// get current color
    const vector4& GetColor() const;
    /// set uvs coordinates
    void SetUvs (vector2& topLeft, vector2& bottomRight);
    /// get top-left uvs coordinate
    const vector2& GetUvsTopLeft() const;
    /// get bottom-right uvs coordinate
    const vector2& GetUvsBottomRight() const;
    /// render the widget
    virtual bool Render();

private:
    nRef<nTexture2> refTexture;
    vector4 color;
    vector2 uvsTopLeft;
    vector2 uvsBottomRight;
};

//------------------------------------------------------------------------------
/**
    Set the texture, a 0 pointer is valid if nothing should be rendered.
*/
inline
void
nGuiTextureView::SetTexture(nTexture2* tex)
{
    this->refTexture = tex;
}

//------------------------------------------------------------------------------
/**
    Set the color to multiply with the texture
*/
inline
void
nGuiTextureView::SetColor(vector4& colorToSet)
{
    this->color = colorToSet;
}

//------------------------------------------------------------------------------
/**
    Get the current color to multiply with the texture
*/
inline
const vector4&
nGuiTextureView::GetColor() const
{
    return this->color;
}

//------------------------------------------------------------------------------
/**
    Set the uvs coordinates
*/
inline
void
nGuiTextureView::SetUvs(vector2& topLeft, vector2& bottomRight)
{
    this->uvsTopLeft = topLeft;
    this->uvsBottomRight = bottomRight;
}

//------------------------------------------------------------------------------
/**
    Get the top-left uvs coordinate
*/
inline
const vector2&
nGuiTextureView::GetUvsTopLeft() const
{
    return this->uvsTopLeft;
}

//------------------------------------------------------------------------------
/**
    Get the bottom-right uvs coordinate
*/
inline
const vector2&
nGuiTextureView::GetUvsBottomRight() const
{
    return this->uvsBottomRight;
}

//------------------------------------------------------------------------------
#endif

