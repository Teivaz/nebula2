#ifndef N_GUITEXTUREVIEW_H
#define N_GUITEXTUREVIEW_H
//------------------------------------------------------------------------------
/**
    A label widget which directly renders a texture. Used by the texture
    browser.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

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
    /// render the widget
    virtual bool Render();

private:
    nRef<nTexture2> refTexture;
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
#endif
    
