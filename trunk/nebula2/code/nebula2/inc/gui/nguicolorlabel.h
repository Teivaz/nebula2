#ifndef N_GUICOLORLABEL_H
#define N_GUICOLORLABEL_H
//------------------------------------------------------------------------------
/**
    @class nGuiColorLabel
    @ingroup Gui

    @brief A label widget which screens one color.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

class nTexture2;

//------------------------------------------------------------------------------
class nGuiColorLabel : public nGuiWidget
{
public:
    /// constructor
    nGuiColorLabel();
    /// destructor
    virtual ~nGuiColorLabel();
    /// set color to display
    void SetColor(vector4& colorToSet);
    /// get current color
    const vector4& GetColor() const;
    /// on show
    virtual void OnShow();
    /// on hide
    virtual void OnHide();
    /// render the widget
    virtual bool Render();

private:
    nRef<nTexture2> refTexture;
    vector4 color;
};


//------------------------------------------------------------------------------
/**
    Set the color to display
*/
inline
void
nGuiColorLabel::SetColor(vector4& colorToSet)
{
    this->color = colorToSet;
}

//------------------------------------------------------------------------------
/**
    Get the current color to display
*/
inline
const vector4&
nGuiColorLabel::GetColor() const
{
    return this->color;
}

//------------------------------------------------------------------------------
#endif

